// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/memory_manager.h"

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
  uint64_t total_mem;
  uint64_t used_mem;
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
  if (block == NULL) {
    return;
  }
  uint8_t order = block->order;
  block_t * curr_block = buddy_man.free_blocks[order];
  if (curr_block == NULL) {
    return;
  }

  if (curr_block == block) {
    buddy_man.free_blocks[order]->status = OCCUPIED;
    buddy_man.free_blocks[order] = buddy_man.free_blocks[order]->next;
    return;
  }

  while (curr_block->next != NULL && curr_block->next != block) {
    curr_block = curr_block->next;
  }

  if (curr_block->next == NULL) {
    return;
  }

  curr_block->next = curr_block->next->next;
  curr_block->status = OCCUPIED;
  
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
    return NULL; // @todo: handle error
  }

  block_t * left = block < buddy_block ? block : buddy_block;
  remove_block(buddy_block);
  left->order++;
  left->status = FREE;
  return left;
}
  
void start_mm() {
  int current_size = 2;
  int level = 1;
  while (current_size < MEM_SIZE) {
      level++;
      current_size *= 2;
  }
  buddy_man.max_order = level; // me guardo el ultimo nivel real
  buddy_man.total_mem = MEM_SIZE;
  buddy_man.used_mem = 0;
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

  buddy_man.used_mem += (1L << order);
  return (void *)((uint8_t *) block + sizeof(block_t));
}

void my_free (void * address) {
  block_t * block = (block_t *) (address - sizeof(block_t));
  if (block == NULL || block->status == FREE) {
    return;
  }
  block->status = FREE;
  
  uint64_t block_pos = (uint64_t) ((void *) block - MEM_START);
  block_t * buddy_block = (block_t *) ((uint64_t) MEM_START + (((uint64_t) block_pos) ^ ((1L << block->order))));
  if (buddy_block == NULL) {
    return;
  }
  while (block && buddy_block && block->order < buddy_man.max_order && buddy_block->status == FREE && buddy_block->order == block->order) {
    block = merge(block, buddy_block);
    if (block == NULL) {  // excessive checking to avoid PVS warning
      return;
    }
    block->status = FREE;
    block_pos = (uint64_t)((void *) block - ((uint64_t) MEM_START));
    buddy_block = (block_t *) (block_t *) ((uint64_t) MEM_START + (((uint64_t) block_pos) ^ ((1L << block->order))));
  }

  if (block == NULL) {
    return;
  }
  create_block((void *) block, block->order);

  buddy_man.used_mem -= (1L << block->order);
  return;
}

void mem(mem_info * info) {
  info->total = buddy_man.total_mem;
  info-> free = buddy_man.total_mem - buddy_man.used_mem;
  info->used = buddy_man.used_mem;
}
