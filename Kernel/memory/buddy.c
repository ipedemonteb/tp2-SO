#include "buddy.h"

#define MAX_ORDER 25 // maximo nivel hardcodeado (por el tamaño de la memoria es el maximo nivel que deberia tener, lo pongo como proteccion x las)

// @ todo: ver si deberia hacer validaciones de los parametros en cada funcion o no

typedef struct block_t {
    int8_t order;
    int8_t status;
    struct block_t * next;
} block_t;

typedef struct buddy_manager {
    int8_t max_order;
    block_t * free_blocks[MAX_ORDER];   // arreglo de listas de bloques libres x nivel. en el arreglo hay un solo bloque representativo que arranca la lista
} buddy_manager;

buddy_manager buddy_man;

block_t * create_block(void * address, int8_t order) {
  block_t * block = (block_t *) address;
  block->order = order;
  block->status = FREE;
  block->next = buddy_man.free_blocks[order]; // hago que el next sea el current -> voy agregando "al principio"
  buddy_man.free_blocks[order] = block;  // me pongo como representante de ese nivel
  return block;
}

void remove_block(block_t * block) {
  block_t ** head = &buddy_man.free_blocks[block->order];
  if (*head == block) { // el bloque es el primero en la lista
    buddy_man.free_blocks[block->order] = block->next;
  } else {
    block_t * prev = *head;
    while (prev && prev->next != block) {   // itero por la lista hasta encontrar el bloque anterior al que quiero sacar
      prev = prev->next;
    }
    if (prev) {
      prev->next = block->next;
    }
  }
  block->next = NULL;
  return;
}

void split_block(int8_t order) {
  block_t * block = buddy_man.free_blocks[order];
  remove_block(block); // saco al bloque del nivel en el que estaba (pues ya no existe más ese bloque)
  block_t * buddy_block = (block_t *) ((void *) block + (1L << (order - 1))); // obtengo el buddy del bloque @check order o order-1
  create_block((void *) buddy_block, order - 1); // creo el bloque buddy
  create_block((void *) block, order - 1); // creo el bloque original

  return;
}

block_t * merge(block_t * block, block_t * buddy_block) { // a esta funcion se la llama ya habiendo chequeado que el buddy este libre y sea del mismo nivel que el block, por lo que tiene que mergear a ciegas
  if (block == NULL || buddy_block == NULL) {
    return; // @todo: handle error
  }

  block_t * left = block < buddy_block ? block : buddy_block;
  remove_block(buddy_block);
  left->order++;
  left->status = FREE;
  return left;
}
  
void init_buddy() {
  int current_size = 2;
  int level = 1;
  while (current_size < MEM_SIZE) {
      level++;
      current_size *= 2;
  }
  buddy_man.max_order = level; // me guardo el ultimo nivel real

  for (int i=0; i < MAX_ORDER; i++) {
    buddy_man.free_blocks[i] = NULL;
  }
  create_block((void *) MEM_START, level);
}

void * my_malloc(uint64_t size) {
  if (size <= 0 || size > MEM_SIZE) {
    return NULL; // @todo: handle error
  }

  // encuentro el orden ideal
  int8_t order = 1;
  int64_t block_size = 2;
  while (block_size < size + sizeof(block_t) && order < buddy_man.max_order) {
    order++;
    block_size *= 2;
  }

  if (buddy_man.free_blocks[order] == NULL) { // no hay definidos bloques libres de ese tamaño (o estan ocupados por ende no estan en la lista o nunca se crearon)
    uint8_t order_approx = 0;
    // busco el nivel superior más cercano disponible para luego splittearlo
    for (uint8_t i = order + 1; i <= buddy_man.max_order && !order_approx; i++) {
      if (buddy_man.free_blocks[i] != NULL) {
        order_approx = i;
      }
    }

    if (order_approx == 0) {
      drawString("could not malloc", 20, 20, RED, BLACK);
      return NULL;
    }

    // splitteo bloques hasta llegar al orden deseado
    while (order_approx > order) {
      split_block(order_approx);
      order_approx--;
    }
  }

  block_t * block = buddy_man.free_blocks[order]; // el bloque que voy a devolver es el que cree en el level encontrado inicialmente
  remove_block(block); // saco al bloque del nivel en el que estaba (pues estará ocupado ya que lo devuelvo en el malloc)
  block->status = OCCUPIED;

  print_memory(); // @todo: borrar

  return (void *) block + sizeof(block_t);

}

void my_free (void * address) {
  block_t * block = (block_t *) (address - sizeof(block_t));
  if (block->status == FREE) {
    return; // @todo: handle error
  }
  block->status = FREE;
  
  uint64_t block_pos = (uint64_t) ((void *) block - MEM_START);
  block_t * buddy_block = (block_t *) (MEM_START + (block_pos ^ (1L << block->order)));

  while (buddy_block->status == FREE && buddy_block->order == block->order && block->order < buddy_man.max_order) {
    block = merge(block, buddy_block);
    block_pos = (uint64_t) ((void *) block - MEM_START);
    buddy_block = (block_t *) (MEM_START + (block_pos ^ (1L << block->order)));
  }

  create_block((void *) block, block->order);

  print_memory(); // @todo: borrar

  return;
}

void print_memory() {
    char buffer[32];
    int x_pos = 0; // Start each level at the left side of the screen
    int y_pos = 4; // Starting y position

    for (int level = 0; level < 6; level++) {
        // Print the level header
        drawString("Level: ", x_pos, y_pos, WHITE, BLACK);
        int len = int_to_str(level, buffer);
        drawString((uint8_t *) buffer, x_pos + 20, y_pos, WHITE, BLACK);

        block_t * current_block = buddy_man.free_blocks[level];
        int block_x_pos = 200; // Start blocks info a bit further right

        // Loop through each block in the level
        while (current_block != NULL) {
            // Print each block's starting address
            drawString("Addr: ", block_x_pos, ++y_pos, GREEN, BLACK);
            len = addr_to_str((uint64_t) current_block, buffer);
            drawString((uint8_t *) buffer, block_x_pos + 60, y_pos, GREEN, BLACK);

            current_block = current_block->next;
            block_x_pos += 200; // Space each block's info further to the right
        }
        drawString("End of level", block_x_pos, ++y_pos, WHITE, BLACK);

        y_pos++; // Move to the next line for the next level
    }
}

// Helper function to convert integer to string
int int_to_str(int num, char *buffer) {
    int len = 0;
    if (num == 0) {
        buffer[len++] = '0';
    } else {
        int temp = num;
        while (temp > 0) {
            buffer[len++] = '0' + (temp % 10);
            temp /= 10;
        }
        // Reverse the string
        for (int i = 0; i < len / 2; i++) {
            char swap = buffer[i];
            buffer[i] = buffer[len - i - 1];
            buffer[len - i - 1] = swap;
        }
    }
    buffer[len] = '\0';
    return len;
}

// Helper function to convert address to hex string
int addr_to_str(uint64_t addr, char *buffer) {
    int len = 0;
    buffer[len++] = '0';
    buffer[len++] = 'x';
    for (int i = (sizeof(addr) * 8) - 4; i >= 0; i -= 4) {
        int hex_digit = (addr >> i) & 0xF;
        buffer[len++] = (hex_digit < 10) ? '0' + hex_digit : 'A' + (hex_digit - 10);
    }
    buffer[len] = '\0';
    return len;
}


