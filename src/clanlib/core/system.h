/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS >AS IS>
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <ClanLib/Core/System/databuffer.h>
#include <ClanLib/Core/System/block_allocator.h>
#include <ClanLib/Core/System/console_window.h>
#include <ClanLib/Core/System/datetime.h>
#include <ClanLib/Core/System/disposable_object.h>
#include <ClanLib/Core/System/event.h>
#include <ClanLib/Core/System/event_provider.h>
#include <ClanLib/Core/System/exception.h>
#include <ClanLib/Core/System/mutex.h>
#include <ClanLib/Core/System/runnable.h>
#include <ClanLib/Core/System/service.h>
#include <ClanLib/Core/System/setup_core.h>
#include <ClanLib/Core/System/thread.h>
#include <ClanLib/Core/System/thread_local_storage.h>
#include <ClanLib/Core/System/autoptr.h>
#include <ClanLib/Core/System/weakptr.h>
#include <ClanLib/Core/System/sharedptr.h>
#include <ClanLib/Core/System/system.h>
#include <ClanLib/Core/System/command_line.h>
#include <ClanLib/Core/System/memory_pool.h>
#include <ClanLib/Core/System/static_memory_pool.h>
#include <ClanLib/Core/System/fixed_memory_pool.h>
#include <ClanLib/Core/System/keep_alive.h>
#include <ClanLib/Core/System/timer.h>
#include <ClanLib/Core/System/registry_key.h>
#include <ClanLib/Core/System/interlocked_variable.h>
