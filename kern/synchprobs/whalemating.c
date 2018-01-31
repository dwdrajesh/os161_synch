/*
 * Copyright (c) 2001, 2002, 2009
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

/*
 * Driver code is in kern/tests/synchprobs.c We will
 * replace that file. This file is yours to modify as you see fit.
 *
 * You should implement your solution to the whalemating problem below.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

/*
 * Called by the driver during initialization.
 */




/* My solution:
/////////////////////////////////////
---- Solution: Use sem and locks
1. Keep 3 locks: male, female and mm
2. 3 semaphores each start with 0
3. In each male, female and mm, call the _start() function without lock
4. Acquire the lock now after returning from _start()
5. Increase the own semaphore 2 times (since female and mm will decrease each )
6. Decrease female and mm semaphores
7. Call _end() function
8. Release lock 
/////////////////////////////////////
*/

static struct lock *m_lock;
static struct lock *fem_lock;
static struct lock *mm_lock;

static struct semaphore *male_sem;
static struct semaphore *female_sem;
static struct semaphore *mm_sem;

int male_count;
int female_count;
int mm_count;

void whalemating_init() {
	m_lock = lock_create("male");
	KASSERT(m_lock != NULL);
	fem_lock = lock_create("female");
	KASSERT(fem_lock != NULL);
	mm_lock = lock_create("m");
	KASSERT(mm_lock != NULL);

	male_sem = sem_create("m_sem", 0);
	KASSERT(male_sem != NULL);
	female_sem = sem_create("fem_sem", 0);
	KASSERT(female_sem != NULL);
	mm_sem = sem_create("mm_sem", 0);
	KASSERT(mm_sem != NULL);
	return;
}

/*
 * Called by the driver during teardown.
 */

void
whalemating_cleanup() {
	return;
}

void
male(uint32_t index)
{
	//(void)index;
	/*
	 * Implement this function by calling male_start and male_end when
	 * appropriate.
	 */
	male_start(index);

	lock_acquire(m_lock);
	V(male_sem);
	V(male_sem);

	
	P(female_sem);
	P(mm_sem);


	male_end(index);

	kprintf_n("Male_end: %d\n", index);
	lock_release(m_lock);
	return;
}

void
female(uint32_t index)
{
	//(void)index;
	/*
	 * Implement this function by calling female_start and female_end when
	 * appropriate.
	 */
	
	
	female_start(index);

	lock_acquire(fem_lock);
	V(female_sem);
	V(female_sem);

	P(male_sem);
	P(mm_sem);


	female_end(index);
	kprintf_n("Female_end: %d\n", index);
	lock_release(fem_lock);

	return;
}

void
matchmaker(uint32_t index)
{
	//(void)index;
	/*
	 * Implement this function by calling matchmaker_start and matchmaker_end
	 * when appropriate.
	 */
	
	matchmaker_start(index);

	lock_acquire(mm_lock);

	V(mm_sem);
	V(mm_sem);

	kprintf_n("Matchmaker_start wait for male: %d\n", index);
	P(male_sem);
	kprintf_n("Matchmaker_start wait for female: %d\n", index);
	P(female_sem);

	kprintf_n("Matchmaker_start exit: %d\n", index);

	matchmaker_end(index);

	kprintf_n("Matchmaker_end: %d\n", index);
	lock_release(mm_lock);

	return;
}
