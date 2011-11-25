void realinitmips(unsigned int msize);
void enable_cache()
{
	    __asm__ volatile(
		".set mips2;\n" \
"        mfc0   $4,$16;\n" \
"        and    $4,$4,0xfffffff8;\n" \
"        or     $4,$4,0x3;\n" \
"        mtc0   $4,$16;\n" \
"		.set mips0;\n"
		::
		:"$4"
		);
}

#ifndef NOCACHE2
void flush_cache2()
{
asm volatile(
".set mips3;\n" \
"	mfc0	$3, $15;			# read processor ID register;\n" \
" 	li		$2, 0x6303;				#godson2f prid;\n" \
" 	beq		$2, $3, godson_2f;\n" \
" 	nop;\n" \
"	li		$2, 0x6302;				#godson2e prid;\n" \
"	bne	$2, $3,11f ;\n" \
"	nop;\n" \
"# godson2e;\n" \
" godson_2f: " \
"	li	  $2, 0x80000000;\n" \
"   addu  $3,$2,512*1024/4;\n" \
"10:\n" \
"	cache	3, 0($2);\n" \
"	cache	3, 1($2);\n" \
"	cache	3, 2($2);\n" \
"	cache	3, 3($2);\n" \
"	addu	$2, 32;\n" \
"	bne	    $2,$3, 10b;\n" \
"	nop;\n" \
"11:\n" \
:::"$2","$3"
);
}
#else
void flush_cache()
{

#ifndef WAYBIT
#define WAYBIT 0
#endif
#define WAY__(x) #x
#define WAY_(x,y) WAY__((x<<y))
#define WAY(x) WAY_(x,WAYBIT)
asm volatile(
"		.set mips3;\n" 
"        li    $5,0x80000000;\n" 
"        addu  $6,$5,16384;\n" 
"1:\n" 
"        cache  1," WAY(0) "($5);\n" 
"        cache  1," WAY(1) "($5);\n" 
"        cache  1," WAY(2) "($5);\n" 
"        cache  1," WAY(3) "($5);\n" 
"        cache  0," WAY(0) "($5);\n" 
"        cache  0," WAY(1) "($5);\n" 
"        cache  0," WAY(2) "($5);\n" 
"        cache  0," WAY(3) "($5);\n" 
"        add    $5,$5,32;\n" 
"        bne    $5,$6,1b;\n" 
"        nop;\n" 
"		.set mips0;\n" 
::: "$5","$6");
}
#endif
void initmips(unsigned int msize,int dmsize,int dctrl)
{
	long *edata=(void *)0x800d412c;
	long *end=(void *)0x800f9834;
	int *p;
	int debug=(msize==0);
//	CPU_TLBClear();
	tgt_puts("Uncompressing Bios");
//	if(!debug||dctrl&1)enable_cache();
	
	while(1){
		if(run_unzip(biosdata,0x80010000)>=0)break;
	}
	tgt_puts("OK,Booting Bios\r\n");
	for(p=edata;p<=end;p++){
		*p=0;
	}
	memset(0x80010000-0x1000,0,0x1000);//0x80010000-0x1000 for frame(registers),memset for pretty
#ifdef NOCACHE2
//	flush_cache();
#else
//	flush_cache2();
#endif
	realinitmips(debug?dmsize:msize);
}


void realinitmips(unsigned int msize)
{
	     asm ("li  $29,0x80010000-0x4000;\n" \
"		       li $2,0x8008d824;\n" \
"			   move $4,%0;\n" \
"			   jalr $2;\n" \
"			   nop;\n" \
"			  1: b 1b;nop;" \
          :
          : "r" (msize)
          : "$29", "$2","$4");

}
void tgt_puts(char *str)
{
	while(*str)
	tgt_putchar(*str++);
//	return 0;
}
