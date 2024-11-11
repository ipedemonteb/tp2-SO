#include "memory_manager.h"

#define MIN_LEVEL 5 // nivel minimo de la memoria (2^5 = 32 bytes)
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
  uint8_t order = block->order;
  if (block == NULL) {
    // printf("Error: trying to remove a NULL block.\n");
    return;
  }

  block_t * curr_block = buddy_man.free_blocks[order];
  if (curr_block == NULL) {
    // printf("Error in rem_block: level: %ld does not have free blocks\n",
    // level);
    return;
  }

  if (curr_block == block) {
    buddy_man.free_blocks[order]->status = OCCUPIED;
    buddy_man.free_blocks[order] = buddy_man.free_blocks[order]->next;
    return;
  }
  // Si llegue hasta aca: hay primero y no es el bloque

  while (curr_block->next != NULL && curr_block->next != block) {
    curr_block = curr_block->next;
  }
  if (curr_block->next == NULL) {
    // printf("Error: culd not find the block in the specified level\n");
    return;
  }

  curr_block->next = curr_block->next->next;

  curr_block->status = OCCUPIED;
  return;
  // block_t ** head = &buddy_man.free_blocks[block->order];
  // if (*head == block) { // el bloque es el primero en la lista
  //   buddy_man.free_blocks[block->order] = block->next;
  // } else {
  //   block_t * prev = *head;
  //   while (prev && prev->next != block) {   // itero por la lista hasta encontrar el bloque anterior al que quiero sacar
  //     prev = prev->next;
  //   }
  //   if (prev) {
  //     prev->next = block->next;
  //   }
  // }
  // block->next = NULL;
  // return;
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
    return NULL; // @todo: handle error
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
  return;
}

void * my_malloc(uint64_t size) {
  if (size <= 0 || size > MEM_SIZE) {
    return NULL; // @todo: handle error
  }

  // encuentro el orden ideal
  int8_t order = 1;
  int64_t block_size = 2;
  while (block_size < size + sizeof(block_t)) {
    order++;
    block_size *= 2;
  }

  order = (MIN_LEVEL > order) ? MIN_LEVEL : order; // si el tamaño pedido es menor al tamaño minimo, devuelvo el tamaño minimo

  if (buddy_man.free_blocks[order] == NULL) { // no hay definidos bloques libres de ese tamaño (o estan ocupados por ende no estan en la lista o nunca se crearon)
    uint8_t order_approx = 0;
    // busco el nivel superior más cercano disponible para luego splittearlo
    for (uint8_t i = order + 1; i <= buddy_man.max_order && order_approx == 0; i++) {
      if (buddy_man.free_blocks[i] != NULL) {
        order_approx = i;
      }
    }

    if (order_approx == 0) {
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
  block->status = OCCUPIED; // todo: ver si esto va o no

  return (void *)((uint8_t *) block + sizeof(block_t));
}

void my_free (void * address) {
  block_t * block = (block_t *) (address - sizeof(block_t));
  if (block->status == FREE) {
    return; // @todo: handle error
  }
  block->status = FREE;
  
  uint64_t block_pos = (uint64_t) ((void *) block - MEM_START);
  block_t * buddy_block = (block_t *) ((uint64_t) MEM_START + (((uint64_t) block_pos) ^ ((1L << block->order))));

  while (block->order < buddy_man.max_order && buddy_block->status == FREE && buddy_block->order == block->order) {
    block = merge(block, buddy_block);
    block->status = FREE;
    block_pos = (uint64_t)((void *) block - ((uint64_t) MEM_START));
    buddy_block = (block_t *) (block_t *) ((uint64_t) MEM_START + (((uint64_t) block_pos) ^ ((1L << block->order))));
  }

  create_block((void *) block, block->order);

  return;
}
