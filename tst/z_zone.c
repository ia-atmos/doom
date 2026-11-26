// The test platform: memory. It takes the place of engine/src/z_zone.c.
//
// The blocks are cleared, one by one, and purgable blocks (PU_CACHE)
// are never purged, so that a run is the same on any machine and in any
// build.
//
// The original engine read past the end of some blocks (a column of a
// sprite, of a short texture), and that garbage reached the screen; the
// copy in engine/ has those reads fixed (doc/bugs.md). A build with
// AddressSanitizer checks that none is left (doc/bugs.md has the
// command); for it, TST_TAIL=0 leaves no bytes after a block, which is
// now the default.

#include <stdlib.h>
#include <string.h>

#include "doomtype.h"
#include "i_system.h"
#include "z_zone.h"

#define TST_PAD 256

// The cleared bytes after a block: none.
#ifndef TST_TAIL
#define TST_TAIL 0
#endif
#define TST_ID  0x1d4a11

typedef struct tst_block_s
{
    int size;
    int tag;
    int id;
    void **user;
    struct tst_block_s *prev;
    struct tst_block_s *next;
    byte pad[TST_PAD];
} tst_block_t;

static tst_block_t tst_blocks;      // the head of a circular list

static tst_block_t *TST_Block (void *ptr, const char *who)
{
    tst_block_t *block = (tst_block_t *) ((byte *) ptr - sizeof(tst_block_t));

    if (block->id != TST_ID)
    {
        I_Error("%s: a pointer without the id of a block", who);
    }

    return block;
}

void Z_Init (void)
{
    tst_blocks.prev = &tst_blocks;
    tst_blocks.next = &tst_blocks;
}

void *Z_Malloc (int size, int tag, void *user)
{
    tst_block_t *block;
    void *result;

    if (tag >= PU_PURGELEVEL && user == NULL)
    {
        I_Error("Z_Malloc: an owner is required for purgable blocks");
    }

    block = calloc(1, sizeof(tst_block_t) + size + TST_TAIL);

    if (block == NULL)
    {
        I_Error("Z_Malloc: failed on allocation of %i bytes", size);
    }

    block->size = size;
    block->tag = tag;
    block->id = TST_ID;
    block->user = user;
    block->prev = tst_blocks.prev;
    block->next = &tst_blocks;
    block->prev->next = block;
    block->next->prev = block;

    result = (byte *) block + sizeof(tst_block_t);

    if (user != NULL)
    {
        *block->user = result;
    }

    return result;
}

void Z_Free (void *ptr)
{
    tst_block_t *block = TST_Block(ptr, "Z_Free");

    if (block->user != NULL)
    {
        *block->user = NULL;
    }

    block->prev->next = block->next;
    block->next->prev = block->prev;
    block->id = 0;
    free(block);
}

void Z_FreeTags (int lowtag, int hightag)
{
    tst_block_t *block = tst_blocks.next;

    while (block != &tst_blocks)
    {
        tst_block_t *next = block->next;

        if (block->tag >= lowtag && block->tag <= hightag)
        {
            Z_Free((byte *) block + sizeof(tst_block_t));
        }

        block = next;
    }
}

void Z_ChangeTag2 (void *ptr, int tag, const char *file, int line)
{
    tst_block_t *block = TST_Block(ptr, "Z_ChangeTag");

    if (tag >= PU_PURGELEVEL && block->user == NULL)
    {
        I_Error("%s:%i: Z_ChangeTag: an owner is required "
                "for purgable blocks", file, line);
    }

    block->tag = tag;
}

void Z_ChangeUser (void *ptr, void **user)
{
    tst_block_t *block = TST_Block(ptr, "Z_ChangeUser");

    block->user = user;
    *user = ptr;
}

void Z_CheckHeap (void)
{
    tst_block_t *block;

    for (block = tst_blocks.next; block != &tst_blocks; block = block->next)
    {
        if (block->id != TST_ID || block->next->prev != block)
        {
            I_Error("Z_CheckHeap: a block is damaged");
        }
    }
}

void Z_DumpHeap (int lowtag, int hightag)
{
}

void Z_FileDumpHeap (FILE *f)
{
}

int Z_FreeMemory (void)
{
    return 0;
}

unsigned int Z_ZoneSize (void)
{
    return 0;
}
