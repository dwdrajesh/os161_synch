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
 * Driver code is in kern/tests/synchprobs.c We will replace that file. This
 * file is yours to modify as you see fit.
 *
 * You should implement your solution to the stoplight problem below. The
 * quadrant and direction mappings for reference: (although the problem is, of
 * course, stable under rotation)
 *
 *   |0 |
 * -     --
 *    01  1
 * 3  32
 * --    --
 *   | 2|
 *
 * As way to think about it, assuming cars drive on the right: a car entering
 * the intersection from direction X will enter intersection quadrant X first.
 * The semantics of the problem are that once a car enters any quadrant it has
 * to be somewhere in the intersection until it call leaveIntersection(),
 * which it should call while in the final quadrant.
 *
 * As an example, let's say a car approaches the intersection and needs to
 * pass through quadrants 0, 3 and 2. Once you call inQuadrant(0), the car is
 * considered in quadrant 0 until you call inQuadrant(3). After you call
 * inQuadrant(2), the car is considered in quadrant 2 until you call
 * leaveIntersection().
 *
 * You will probably want to write some helper functions to assist with the
 * mappings. Modular arithmetic can help, e.g. a car passing straight through
 * the intersection entering from direction X will leave to direction (X + 2)
 * % 4 and pass through quadrants X and (X + 3) % 4.  Boo-yah.
 *
 * Your solutions below should call the inQuadrant() and leaveIntersection()
 * functions in synchprobs.c to record their progress.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

/*
 * Called by the driver during initialization.
 */
	
#define st 0
#define rt 1
#define lt 2

struct semaphore* sem_array[4];
struct semaphore* leave_sem[4];

void mapping(int index, int dir, int *map)
{
	switch(dir)
	{
	case st:
		*map = index;
		*(map + 1) = (index + 3) % 4; 		
		break;

	case rt:
		*map = index;
		break;

	case lt:
		*map = index;
		*(map + 1) = (index + 3) % 4; 		
		*(map + 2) = (index + 2) % 4; 		
		break;
	default:
		kprintf_n("-x-x-x-Error in map\n");
		break;
	}
	kprintf_n("----x-x-x- coming direction: %d, going is: %d\n",
		index, dir);
	//kprintf_n("----x-x-x- map: %d, going is: %s\n");
}

void
stoplight_init() {
	//int i;
	//for(i = 0; i < 4; i++)
	//{
		sem_array[0] = sem_create("temp", 1);
		KASSERT(sem_array[0] != NULL);
	//}

	sem_array[1] = sem_create("temp1", 1);
	KASSERT(sem_array[1] != NULL);
	sem_array[2] = sem_create("temp2", 1);
	KASSERT(sem_array[2] != NULL);
	sem_array[3] = sem_create("temp3", 1);
	KASSERT(sem_array[3] != NULL);

	leave_sem[0] = sem_create("temp1", 1);
	KASSERT(leave_sem[0] != NULL);
	leave_sem[1] = sem_create("temp1", 1);
	KASSERT(leave_sem[1] != NULL);
	leave_sem[2] = sem_create("temp1", 1);
	KASSERT(leave_sem[2] != NULL);
	leave_sem[3] = sem_create("temp1", 1);
	KASSERT(leave_sem[3] != NULL);
	return;
}

/*
 * Called by the driver during teardown.
 */

void stoplight_cleanup() {
	return;
}

void
turnright(uint32_t direction, uint32_t index)
{
	//(void)direction;
	//(void)index;
	/*
	 * Implement this function.
	 */
	int map = 0;
	mapping(direction, rt, &map);
	kprintf_n("map: %d\n", map); 
	
	P(sem_array[map]);
	inQuadrant(map, index);
	
	P(leave_sem[direction]);

	leaveIntersection(index, rt, direction);
	V(sem_array[map]);

	V(leave_sem[direction]);
	return;
}
void
gostraight(uint32_t direction, uint32_t index)
{
	//(void)direction;
	//(void)index;
	/*
	 * Implement this function.
	 */
	int map[2] = {0};
	mapping(direction, st, map);
	
	kprintf_n("map[0]: %d, map[1]: %d\n", map[0], map[1]);
	P(sem_array[map[0]]);
	inQuadrant(map[0], index);
	
	P(sem_array[map[1]]);

	inQuadrant(map[1], index);
	V(sem_array[map[0]]);

	P(leave_sem[direction]);

	V(sem_array[map[1]]);
	leaveIntersection(index, st, direction);

	V(leave_sem[direction]);
	return;
}
void
turnleft(uint32_t direction, uint32_t index)
{
	//(void)direction;
	//(void)index;
	/*
	 * Implement this function.
	 */
	int map[3] = {0};
	mapping(direction, lt, map);

	kprintf_n("map[0]: %d, map[1]: %d, map[2]: %d\n", map[0], map[1], map[2]);
	P(sem_array[map[0]]);
	inQuadrant(map[0], index);
	
	P(sem_array[map[1]]);
	inQuadrant(map[1], index);

	V(sem_array[map[0]]);

	P(sem_array[map[2]]);

	inQuadrant(map[2], index);
	V(sem_array[map[1]]);

	P(leave_sem[direction]);

	leaveIntersection(index, lt, direction);
	V(sem_array[map[2]]);

	V(leave_sem[direction]);
	return;
}
