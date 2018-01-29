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

static struct lock *m_lock;
static struct lock *fem_lock;
static struct lock *mm_lock;
static struct cv *male_cv;
static struct cv *female_cv;
static struct cv *mm_cv;

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

	male_cv = cv_create("m_cv");
	KASSERT(male_cv != NULL);
	female_cv = cv_create("fem_cv");
	KASSERT(female_cv != NULL);
	mm_cv = cv_create("mm_cv");
	KASSERT(mm_cv != NULL);
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
	(void)index;
	/*
	 * Implement this function by calling male_start and male_end when
	 * appropriate.
	 */
	male_start(index);

	lock_acquire(m_lock);
	//increase own count first
	male_count++;
	//signal own cv to let others know
	cv_signal(male_cv, m_lock);

	// wait for other signals
	cv_wait(female_cv, m_lock);
	cv_wait(mm_cv, m_lock);
	
	// after signals received, call end
	// and decrease count
	kprintf_n("Male_end: %d\n", index);
	male_end(index);
	male_count--;
	lock_release(m_lock);
	return;
}

void
female(uint32_t index)
{
	(void)index;
	/*
	 * Implement this function by calling female_start and female_end when
	 * appropriate.
	 */
	
	female_start(index);
	
	lock_acquire(fem_lock);
	
	female_count++;

	cv_signal(female_cv, fem_lock);

	cv_wait(male_cv, fem_lock);
	cv_wait(mm_cv, fem_lock);

	kprintf_n("Female_end: %d\n", index);
	female_end(index);
	female_count--;
	lock_release(fem_lock);

	return;
}

void
matchmaker(uint32_t index)
{
	(void)index;
	/*
	 * Implement this function by calling matchmaker_start and matchmaker_end
	 * when appropriate.
	 */
	matchmaker_start(index);

	kprintf_n("Matchmaker_start exit: %d\n", index);
	lock_acquire(mm_lock);
	
	mm_count++;

	cv_signal(mm_cv, mm_lock);

	cv_wait(male_cv, mm_lock);
	cv_wait(female_cv, mm_lock);

	kprintf_n("Matchmaker_end: %d\n", index);
	
	matchmaker_end(index);
	lock_release(mm_lock);

	return;
}
