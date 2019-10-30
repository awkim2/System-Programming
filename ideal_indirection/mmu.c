/**
 * Ideal Indirection
 * CS 241 - Fall 2019
 * partner with siyiw3
 */
#include "mmu.h"
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

mmu *mmu_create() {
    mmu *my_mmu = calloc(1, sizeof(mmu));
    my_mmu->tlb = tlb_create();
    return my_mmu;
}

void mmu_read_from_virtual_address(mmu *this, addr32 virtual_address,
                                   size_t pid, void *buffer, size_t num_bytes) {
    assert(this);
    assert(pid < MAX_PROCESS_ID);
    assert(num_bytes + (virtual_address % PAGE_SIZE) <= PAGE_SIZE);
    // TODO: Implement me!

    //context swicth
    if (pid != this->curr_pid) {
        tlb_flush(&this->tlb);
        this->curr_pid = pid;
    }
    // check address in segmentation
    if (address_in_segmentations(this->segmentations[pid], virtual_address)==0) {
        mmu_raise_segmentation_fault(this);
        return;
    }
    // check user permissions
    vm_segmentation* segment = find_segment(this->segmentations[pid], virtual_address);
    if (!(segment->permissions & READ)) {
        mmu_raise_segmentation_fault(this);
        return;
    }

    addr32 tlb_vaddr = virtual_address & 0xFFFFF000; 
    page_table_entry* tlb = tlb_get_pte(&this->tlb, tlb_vaddr);

    if (!tlb) {
        mmu_tlb_miss(this);
        page_directory* pd = this->page_directories[pid];
        addr32 pde_vaddr = virtual_address >> (VIRTUAL_ADDR_SPACE - NUM_OFFSET_BITS); // first 10 bits
        page_directory_entry *pde = &pd->entries[pde_vaddr];

        if (!pde->present) {
            mmu_raise_page_fault(this);
            pde->base_addr = ask_kernel_for_frame(NULL) >> NUM_OFFSET_BITS;
            read_page_from_disk((page_table_entry*)pde);
            pde->present = true;
            pde->read_write = true;
            pde->user_supervisor = false;
        }
        page_table *pt = (page_table*) get_system_pointer_from_pde(pde);
        addr32 pte_vaddr = (virtual_address & 0x003FF000) >> NUM_OFFSET_BITS; // middle 10 bits
        tlb = &(pt->entries[pte_vaddr]);
    } else {
        if (!tlb->present) {
            mmu_raise_page_fault(this);
            tlb->base_addr = (ask_kernel_for_frame(tlb) >> NUM_OFFSET_BITS);
            read_page_from_disk((page_table_entry*)tlb);
            tlb->present = true;
            tlb->read_write = true;
            tlb->user_supervisor = true;
        }
    } 

    tlb->accessed = 1;
    memcpy(buffer, (void *)get_system_pointer_from_pte(tlb) + (virtual_address & 0xFFF), num_bytes);
    tlb_add_pte(&this->tlb, tlb_vaddr, tlb);
}

void mmu_write_to_virtual_address(mmu *this, addr32 virtual_address, size_t pid,
                                  const void *buffer, size_t num_bytes) {
    assert(this);
    assert(pid < MAX_PROCESS_ID);
    assert(num_bytes + (virtual_address % PAGE_SIZE) <= PAGE_SIZE);
    // TODO: Implement me!
    if (pid != this->curr_pid) {
        tlb_flush(&this->tlb);
        this->curr_pid = pid;
    }

    if (!address_in_segmentations(this->segmentations[pid], virtual_address)) {
        mmu_raise_segmentation_fault(this);
        return;
    }
  
    vm_segmentation* segment = find_segment(this->segmentations[pid], virtual_address);
    if (!(segment->permissions & WRITE)) {
        mmu_raise_segmentation_fault(this);
        return;
    }

    addr32 tlb_vaddr = virtual_address & 0xFFFFF000;
    page_table_entry* tlb = tlb_get_pte(&this->tlb, tlb_vaddr);

    if (!tlb) {
        mmu_tlb_miss(this);
        page_directory* pd = this->page_directories[pid];
        addr32 pde_base_virtual_addr = (virtual_address) >> (VIRTUAL_ADDR_SPACE - NUM_OFFSET_BITS); 
        page_directory_entry *pde = &pd->entries[pde_base_virtual_addr];
        if (!pde->present) {
            mmu_raise_page_fault(this);
            pde->base_addr = (ask_kernel_for_frame(NULL) >> NUM_OFFSET_BITS);
            pde->read_write = 1;
            pde->present = 1;
            pde->user_supervisor = 0;
        }
        page_table *pt = (page_table *)get_system_pointer_from_pde(pde);
        addr32 pte_base_virtual_addr = (virtual_address & 0x003FF000) >> NUM_OFFSET_BITS;
        tlb = &(pt->entries[pte_base_virtual_addr]);

    }

    if (!tlb->present) {
        mmu_raise_page_fault(this);
        tlb->base_addr = (ask_kernel_for_frame(tlb) >> NUM_OFFSET_BITS);
        read_page_from_disk(tlb);

        tlb->read_write = 1;
        tlb->present = 1;
        tlb->user_supervisor = 1;
    }

    tlb->accessed = 1;
    tlb->dirty = 1;
    memcpy((void *)get_system_pointer_from_pte(tlb) + (virtual_address & 0xFFF), buffer, num_bytes);
    tlb_add_pte(&this->tlb, tlb_vaddr, tlb);
}

void mmu_tlb_miss(mmu *this) {
    this->num_tlb_misses++;
}

void mmu_raise_page_fault(mmu *this) {
    this->num_page_faults++;
}

void mmu_raise_segmentation_fault(mmu *this) {
    this->num_segmentation_faults++;
}

void mmu_add_process(mmu *this, size_t pid) {
    assert(pid < MAX_PROCESS_ID);
    addr32 page_directory_address = ask_kernel_for_frame(NULL);
    this->page_directories[pid] =
        (page_directory *)get_system_pointer_from_address(
            page_directory_address);
    page_directory *pd = this->page_directories[pid];
    this->segmentations[pid] = calloc(1, sizeof(vm_segmentations));
    vm_segmentations *segmentations = this->segmentations[pid];

    // Note you can see this information in a memory map by using
    // cat /proc/self/maps
    segmentations->segments[STACK] =
        (vm_segmentation){.start = 0xBFFFE000,
                          .end = 0xC07FE000, // 8mb stack
                          .permissions = READ | WRITE,
                          .grows_down = true};

    segmentations->segments[MMAP] =
        (vm_segmentation){.start = 0xC07FE000,
                          .end = 0xC07FE000,
                          // making this writeable to simplify the next lab.
                          // todo make this not writeable by default
                          .permissions = READ | EXEC | WRITE,
                          .grows_down = true};

    segmentations->segments[HEAP] =
        (vm_segmentation){.start = 0x08072000,
                          .end = 0x08072000,
                          .permissions = READ | WRITE,
                          .grows_down = false};

    segmentations->segments[BSS] =
        (vm_segmentation){.start = 0x0805A000,
                          .end = 0x08072000,
                          .permissions = READ | WRITE,
                          .grows_down = false};

    segmentations->segments[DATA] =
        (vm_segmentation){.start = 0x08052000,
                          .end = 0x0805A000,
                          .permissions = READ | WRITE,
                          .grows_down = false};

    segmentations->segments[TEXT] =
        (vm_segmentation){.start = 0x08048000,
                          .end = 0x08052000,
                          .permissions = READ | EXEC,
                          .grows_down = false};

    // creating a few mappings so we have something to play with (made up)
    // this segment is made up for testing purposes
    segmentations->segments[TESTING] =
        (vm_segmentation){.start = PAGE_SIZE,
                          .end = 3 * PAGE_SIZE,
                          .permissions = READ | WRITE,
                          .grows_down = false};
    // first 4 mb is bookkept by the first page directory entry
    page_directory_entry *pde = &(pd->entries[0]);
    // assigning it a page table and some basic permissions
    pde->base_addr = (ask_kernel_for_frame(NULL) >> NUM_OFFSET_BITS);
    pde->present = true;
    pde->read_write = true;
    pde->user_supervisor = true;

    // setting entries 1 and 2 (since each entry points to a 4kb page)
    // of the page table to point to our 8kb of testing memory defined earlier
    for (int i = 1; i < 3; i++) {
        page_table *pt = (page_table *)get_system_pointer_from_pde(pde);
        page_table_entry *pte = &(pt->entries[i]);
        pte->base_addr = (ask_kernel_for_frame(pte) >> NUM_OFFSET_BITS);
        pte->present = true;
        pte->read_write = true;
        pte->user_supervisor = true;
    }
}

void mmu_remove_process(mmu *this, size_t pid) {
    assert(pid < MAX_PROCESS_ID);
    // example of how to BFS through page table tree for those to read code.
    page_directory *pd = this->page_directories[pid];
    if (pd) {
        for (size_t vpn1 = 0; vpn1 < NUM_ENTRIES; vpn1++) {
            page_directory_entry *pde = &(pd->entries[vpn1]);
            if (pde->present) {
                page_table *pt = (page_table *)get_system_pointer_from_pde(pde);
                for (size_t vpn2 = 0; vpn2 < NUM_ENTRIES; vpn2++) {
                    page_table_entry *pte = &(pt->entries[vpn2]);
                    if (pte->present) {
                        void *frame = (void *)get_system_pointer_from_pte(pte);
                        return_frame_to_kernel(frame);
                    }
                    remove_swap_file(pte);
                }
                return_frame_to_kernel(pt);
            }
        }
        return_frame_to_kernel(pd);
    }

    this->page_directories[pid] = NULL;
    free(this->segmentations[pid]);
    this->segmentations[pid] = NULL;

    if (this->curr_pid == pid) {
        tlb_flush(&(this->tlb));
    }
}

void mmu_delete(mmu *this) {
    for (size_t pid = 0; pid < MAX_PROCESS_ID; pid++) {
        mmu_remove_process(this, pid);
    }

    tlb_delete(this->tlb);
    free(this);
    remove_swap_files();
}
