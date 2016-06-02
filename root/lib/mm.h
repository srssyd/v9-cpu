enum { // page table entry flags
  PTE_P   = 0x001,       // Present
  PTE_W   = 0x002,       // Writeable
  PTE_U   = 0x004,       // User
//PTE_PWT = 0x008,       // Write-Through
//PTE_PCD = 0x010,       // Cache-Disable
  PTE_A   = 0x020,       // Accessed
  PTE_D   = 0x040,       // Dirty
//PTE_PS  = 0x080,       // Page Size
//PTE_MBZ = 0x180,       // Bits must be zero
};

enum{
    PAGE_SIZE = 4096,
    KERN_BASE = 0xf0000000,
    STACK_SIZE = 0x800000,
};

struct PageInfo{
    struct PageInfo * pp_link;
    uint ref_count;
};

struct PageInfo *page_free_list;

struct PageInfo *pages;

uint mem_size;

char *mem_top;
uint *kpdir;

void *memcpy() { asm(LL,8); asm(LBL, 16); asm(LCL,24); asm(MCPY); asm(LL,8); }
void *memset() { asm(LL,8); asm(LBLB,16); asm(LCL,24); asm(MSET); asm(LL,8); }
void *memchr() { asm(LL,8); asm(LBLB,16); asm(LCL,24); asm(MCHR); }

uint msize()     { asm(MSIZ); }

//Only for boot use.
uint *boot_page_alloc(uint size){
    uint * result = (uint *)mem_top;
    size = (size + PAGE_SIZE -1 ) / PAGE_SIZE * PAGE_SIZE;
    mem_top += PAGE_SIZE;
    return result;
}

//Boot page alloc is not allowed to use after this.
init_page(){
    int i;
    int npages = mem_size / PAGE_SIZE;
    int begin_page = (uint)mem_top / PAGE_SIZE + 1;
    pages = (struct PageInfo *)boot_page_alloc(sizeof(struct PageInfo) * npages);

    page_free_list = 0;

    for(i=0;i<begin_page;i++)
        pages[i].ref_count = 1;

    for(i=begin_page;i<npages;i++){
        pages[i].ref_count = 0;
        pages[i].pp_link = page_free_list;
        page_free_list = pages + i;
    }
}

struct PageInfo * alloc_page(){
    struct PageInfo *page = page_free_list;
    page_free_list = page->pp_link;
    page->ref_count ++;
    return page;
}

release_page(struct PageInfo *page){
    if(--page->ref_count)
        return;
    page->pp_link = page_free_list;
    page_free_list = page;
}

setup_kernel_paging(){

    static int endbss;
    static char kstack[1024];
    int i;
    uint v_addr;
    uint *pde,*pt;
    uint ksp;
    int npages = mem_size / PAGE_SIZE;

    mem_top = (char *)(((uint)&endbss + PAGE_SIZE + PAGE_SIZE - 1) & (-PAGE_SIZE));

    kpdir = boot_page_alloc(PAGE_SIZE);
    memset(kpdir,0,PAGE_SIZE);
    mem_size = msize();


    for(i=0;i<mem_size;i += PAGE_SIZE){
        v_addr = (i + KERN_BASE);
        pde = (uint *)(kpdir + (v_addr >> 22));
        if(*pde & PTE_P)
            pt = *pde & (-PAGE_SIZE);
        else{
            pt = boot_page_alloc(PAGE_SIZE);
            memset(pt,0,PAGE_SIZE);
            *pde = (uint)(pt) | PTE_P | PTE_W;
        }
        pt[((uint)(KERN_BASE + i) >> 12) & 0x3ff] = i | PTE_P | PTE_W;

    }

    //To get the address of page table succefully even after setup page.

    kpdir[0] = kpdir[(uint)KERN_BASE >> 22] ;

}


