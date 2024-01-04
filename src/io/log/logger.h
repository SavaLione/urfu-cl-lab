/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2022-2024 Savelii Pototskii (savalione.com)
 * All rights reserved.
 *
 * Author: Savelii Pototskii <monologuesplus@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file
 * @brief Logger
 * @author Savelii Pototskii (savalione.com)
 * @date 15 Dec 2022
 */
#ifndef IO_LOG_LOGGER_H
#define IO_LOG_LOGGER_H

#include "platform/platform.h"

/* Do we use external fmt library? */
#ifdef NYX_EXTERNAL_FMT
    #define SPDLOG_FMT_EXTERNAL
    #include <spdlog/spdlog.h>
#else
    #include <spdlog/spdlog.h>
#endif

#endif // IO_LOG_LOGGER_H