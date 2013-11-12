/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		event.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "time.h"
#include "atomic.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_event_init()
{
	// make
	tb_size_t* event = tb_malloc0(sizeof(tb_size_t));
	tb_assert_and_check_return_val(event, tb_null);

	// ok
	return (tb_handle_t)event;

fail:
	if (event) tb_free(event);
	return tb_null;
}
tb_void_t tb_event_exit(tb_handle_t handle)
{
	// check
	tb_size_t* event = (tb_size_t*)handle;
	tb_assert_and_check_return(handle);

	// post first
	tb_event_post(handle);

	// wait some time
	tb_msleep(200);

	// free it
	tb_free(event);
}
tb_void_t tb_event_post(tb_handle_t handle)
{
	// check
	tb_size_t* event = (tb_size_t*)handle;
	tb_assert_and_check_return(event);

	// post signal
	tb_atomic_set(event, 1);
}
tb_long_t tb_event_wait(tb_handle_t handle, tb_long_t timeout)
{
	// check
	tb_size_t* event = (tb_size_t*)handle;
	tb_assert_and_check_return_val(event, -1);

	// init
	tb_long_t 	r = 0;
	tb_hong_t 	base = tb_mclock();

	// wait 
	while (1)
	{
		// get post
		tb_size_t post = tb_atomic_fetch_and_set0(event);

		// has signal?
		if (post == 1) 
		{
			r = 1;
			break;
		}
		// error
		else if (post != 0) 
		{
			r = -1;
			break;
		}
		// no signal?
		else
		{
			// timeout?
			if (timeout >= 0 && tb_mclock() - base >= timeout) break;
			else tb_msleep(200);
		}
	}

	return r;
}
