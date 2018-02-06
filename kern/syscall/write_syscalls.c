/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <lib.h>
#include <copyinout.h>
#include <syscall.h>
#include <mips/trapframe.h>
#include <vfs.h>
#include <vnode.h>
#include <uio.h>
#include <current.h> // For 'curproc'
#include <proc.h>

/*
 * Example system call: get the time of day.
 */

// Rajesh: need to copy additional registers starting at sp+16 using copyin() function

// TODO: vfs_open() returns number to identify the file on the inode table (global). Need to create another global file table that keeps track of the file (i.e. mapping from inode to int ) and is global from all processes. 
// Also need to create a per-process file descriptor table
// open() calls sys_open() calls vfs_open()
// the return value from vfs_open, sys_open and open() will be same for now but dup() and fork() might modify the return value from sys_open for per-process

#define PATHLENMAX 64
// Change dbflags to change DB_VM
//DEBUG(DB_VM, "VM free pages: %u\n", free_pages);

//int update_file(int fd) 
//{
//        struct filehandle * fh; 
//        for (fh = curproc->ft->first; fh != NULL; fh = fh->next)
//        {   
//                if (fh->index == fd && fh->mode == 1)
//                {   
//                        struct uio * uio;
//                        uio->uio_offset = fh->offset;
//                        uio->rw = 1; 
//                        return vop_write(fh->vn, fh->offset); 
//                }   
//
//        return 0;
//        }   
//}

int
sys_write(struct trapframe *tf)
{
	//char * filename = (char*) kmalloc(256);
	int fhandle = 0;
	int result;

	result = copyin((const_userptr_t)(tf->tf_a0), &fhandle, 4);
	if(result)
		return -1;	

	int size;
	copyin((const_userptr_t)(tf->tf_a2), &size, 4);
	struct filehandle * fh = proc_find_fd(fhandle);

	struct uio * uio = kmalloc(sizeof(struct uio));
	uio->uio_iov->iov_ubase = (userptr_t)tf->tf_a1;
	uio->uio_iov->iov_len = size;
	uio->uio_iovcnt = 1;
	uio->uio_offset = fh->offset;
	uio->uio_resid = size;
	uio->uio_segflg = UIO_USERSPACE;// user process data
	uio->uio_rw = UIO_WRITE;
	uio->uio_space = (struct addrspace *)tf->tf_a1;

	//vop_write(fh->vn, uio);
	VOP_WRITE(fh->vn, uio);
//	struct vnode *v;
//	// Need to copy a1 first before using directly?
//	result = vfs_open(filename, tf->tf_a1, 0, &v);
//	kprintf("-x-x-x-Opened vfs, mode: %d\n", tf->tf_a1);
//	if (result) {
//		return result;
//	}
//	
//	int rand = proc_create_ft(v);
//
//	//int rand = random();
//	kprintf("-x-x-x-Returning: %d\n", rand);
	return 0;
}
