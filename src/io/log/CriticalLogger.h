/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARX_IO_LOG_CRITICALLOGGER_H
#define ARX_IO_LOG_CRITICALLOGGER_H

#include "io/fs/FileStream.h"
#include "io/log/LogBackend.h"
#include "platform/CrashHandler.h"

namespace logger {

/*!
 * Simple logger that prints plain text to standard output.
 */
class CriticalErrorDialog : public Backend {
	
	std::string errorString;
		
public:
	~CriticalErrorDialog();
	
	void log(const Source & file, int line, Logger::LogLevel level, const std::string & str);
	
	/*!
	 * Returns a CriticalErrorDialog instance if a debugger is attached or NULL otherwise.
	 */
	static CriticalErrorDialog * get();
};

} // namespace logger

#endif // ARX_IO_LOG_CRITICALLOGGER_H
