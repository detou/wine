name	kernel32
type	win32

# Functions exported by the Win95 kernel32.dll 
# (these need to have these exact ordinals, for some win95 dlls 
#  import kernel32.dll by ordinal)
# the base is NOT included in these ordinals

# undocumented ordinal only calls (names taken from k32exp.h by Andrew
# Schulman.
  1 stub VxDCall0
  2 stub VxDCall1
  3 stub VxDCall2
  4 stub VxDCall3
  5 stub VxDCall4
  6 stub VxDCall5
  7 stub VxDCall6
  8 stub VxDCall7
  9 stub VxDCall8
 
 10 stub _KERNEL32_stringconv1 #ansi2oem or reverse?
 
 18 stdcall _KERNEL32_18(long long) _KERNEL32_18
 19 stub _KERNEL32_getheapsegment
 
 31 stub _KERNEL32_31
 
 34 stdcall _KERNEL32_34() _KERNEL32_34
 35 stdcall LoadLibrary16(str) LoadLibrary16
 36 stdcall FreeLibrary16(long) FreeLibrary16
 37 stdcall GetProcAddress16(long str) WIN32_GetProcAddress16
 
 40 register _KERNEL32_40() _KERNEL32_40
 41 stdcall _KERNEL32_41(long str long str str) _KERNEL32_41
 42 stub _KERNEL32_42
 43 stdcall _KERNEL32_43(long str long str str) _KERNEL32_43
 45 register _KERNEL32_45() _KERNEL32_45 
 46 stdcall _KERNEL32_46(long str long str str) _KERNEL32_46
 47 stub _KERNEL32_47
 
 50 stdcall AddAtomA(str) AddAtom32A

 52 stdcall _KERNEL32_52() _KERNEL32_52

# WOW calls
 54 stdcall WOWCallback16(long long) WOWCallback16
 55 stub WOWCallback16Ex
 56 stdcall WOWGetVDMPointer(long long long) WOWGetVDMPointer
 57 stub WOWHandle32
 58 stub WOWHandle16
 59 stdcall WOWGlobalAlloc16(long long) GlobalAlloc16
 60 stdcall WOWGlobalLock16(long) WIN16_GlobalLock16
 61 stdcall WOWGlobalUnlock16(long) GlobalUnlock16
 62 stdcall WOWGlobalFree16(long) GlobalFree16
 63 stdcall WOWGlobalAllocLock16(long long ptr) WOWGlobalAllocLock16
 64 stub WOWGlobalUnlockFree16
 65 stub WOWGlobalLockSize16
 66 stub WOWYield16
 67 stub WOWDirectedYield16
 68 stdcall WOWGetVDMPointerFix(long long long) WOWGetVDMPointerFix
 69 stdcall WOWGetVDMPointerUnfix(long) WOWGetVDMPointerUnfix
 70 stdcall WOW32_1(long long) WOW32_1
 
 72 stub RtlLargeIntegerAdd
 73 stub RtlEnlargedIntegerMultiply
 74 stub RtlEnlargedUnsignedMultiply
 75 stub RtlEnlargedUnsignedDivide
 76 stub RtlExtendedLargeIntegerDivide
 77 stub RtlExtendedMagicDivide
 78 stub RtlExtendedIntegerMultiply
 79 stub RtlLargeIntegerShiftLeft
 80 stub RtlLargeIntegerShiftRight
 81 stub RtlLargeIntegerArithmeticShift
 82 stub RtlLargeIntegerNegate
 83 stub RtlLargeIntegerSubtract
 84 stub RtlConvertLongToLargeInteger
 85 stub RtlConvertUlongToLargeInteger

 87 stdcall _KERNEL32_87() _KERNEL32_87
 88 varargs _KERNEL32_88() _KERNEL32_88
 89 stdcall FT_PrologPrime(ptr ptr) FT_PrologPrime
 90 register _KERNEL32_90() _KERNEL32_90
 91 stub _KERNEL32_91
 92 stub _KERNEL32_92
 93 stdcall GETPWIN16LOCK(ptr) GetPWinLock
 97 stub ENTERSYSLEVEL
 98 stub LEAVESYSLEVEL
 99 stdcall _KERNEL32_98(long) _KERNEL32_98
100 stdcall _KERNEL32_99(long long long) _KERNEL32_99
101 stub _KERNEL32_100


102 stdcall AddAtomW(wstr) AddAtom32W
103 stub AllocConsole
104 stub AllocLSCallback
105 stdcall AllocSLCallback(ptr ptr) AllocSLCallback
106 stdcall AreFileApisANSI() AreFileApisANSI
107 stub BackupRead
108 stub BackupSeek
109 stub BackupWrite
110 stdcall Beep(long long) Beep
111 stub BeginUpdateResourceA
112 stub BeginUpdateResourceW
113 stdcall BuildCommDCBA(str ptr) BuildCommDCB32A
114 stdcall BuildCommDCBAndTimeoutsA(str ptr ptr) BuildCommDCBAndTimeouts32A
115 stdcall BuildCommDCBAndTimeoutsW(wstr ptr ptr) BuildCommDCBAndTimeouts32W
116 stdcall BuildCommDCBW(wstr ptr) BuildCommDCB32W
117 stub CallNamedPipeA
118 stub CallNamedPipeW
119 stub Callback12
120 stub Callback16
121 stub Callback20
122 stub Callback24
123 stub Callback28
124 stub Callback32
125 stub Callback36
126 stub Callback40
127 stub Callback44
128 stub Callback48
129 stub Callback4
130 stub Callback52
131 stub Callback56
132 stub Callback60
133 stub Callback64
134 stub Callback8
135 stdcall ClearCommBreak(long) ClearCommBreak32
136 stdcall ClearCommError(long ptr ptr) ClearCommError
137 stdcall CloseHandle(long) CloseHandle
138 stub CloseProfileUserMapping
139 stub CloseSystemHandle
140 stub CommConfigDialogA
141 stub CommConfigDialogW
142 stdcall CompareFileTime(ptr ptr) CompareFileTime
143 stdcall CompareStringA(long long str long str long) CompareString32A
144 stdcall CompareStringW(long long wstr long wstr long) CompareString32W
145 stub ConnectNamedPipe
146 stdcall ContinueDebugEvent(long long long) ContinueDebugEvent
147 stub ConvertDefaultLocale
148 stdcall ConvertToGlobalHandle(long) ConvertToGlobalHandle
149 stdcall CopyFileA(str str long) CopyFile32A
150 stdcall CopyFileW(wstr wstr long) CopyFile32W
151 stub CreateConsoleScreenBuffer
152 stdcall CreateDirectoryA(str ptr) CreateDirectory32A
153 stdcall CreateDirectoryExA(str str ptr) CreateDirectoryEx32A
154 stdcall CreateDirectoryExW(wstr wstr ptr) CreateDirectoryEx32W
155 stdcall CreateDirectoryW(wstr ptr) CreateDirectory32W
156 stdcall CreateEventA(ptr long long str) CreateEvent32A
157 stdcall CreateEventW(ptr long long wstr) CreateEvent32W
158 stdcall CreateFileA(str long long ptr long long long) CreateFile32A
159 stdcall CreateFileMappingA(long ptr long long long str) CreateFileMapping32A
160 stdcall CreateFileMappingW(long ptr long long long wstr) CreateFileMapping32W
161 stdcall CreateFileW(wstr long long ptr long long long) CreateFile32W
162 stub CreateIoCompletionPort
163 stub CreateKernelThread
164 stub CreateMailslotA
165 stub CreateMailslotW
166 stdcall CreateMutexA(ptr long str) CreateMutex32A
167 stdcall CreateMutexW(ptr long wstr) CreateMutex32W
168 stub CreateNamedPipeA
169 stub CreateNamedPipeW
170 stub CreatePipe
171 stdcall CreateProcessA(str str ptr ptr long long ptr str ptr ptr) CreateProcess32A
172 stdcall CreateProcessW(wstr wstr ptr ptr long long ptr wstr ptr ptr) CreateProcess32W
173 stub CreateRemoteThread
174 stdcall CreateSemaphoreA(ptr long long str) CreateSemaphore32A
175 stdcall CreateSemaphoreW(ptr long long wstr) CreateSemaphore32W
176 stub CreateSocketHandle
177 stub CreateTapePartition
178 stdcall CreateThread(ptr long ptr long long ptr) CreateThread
179 stub CreateToolhelp32Snapshot
180 stub DebugActiveProcess
181 stub DebugBreak
182 stub DefineDosDeviceA
183 stub DefineDosDeviceW
184 stdcall DeleteAtom(long) DeleteAtom32
185 stdcall DeleteCriticalSection(ptr) DeleteCriticalSection
186 stdcall DeleteFileA(str) DeleteFile32A
187 stdcall DeleteFileW(wstr) DeleteFile32W
188 stdcall DeviceIoControl(long long ptr long ptr long ptr ptr) DeviceIoControl
189 stdcall DisableThreadLibraryCalls(long) DisableThreadLibraryCalls
190 stub DisconnectNamedPipe
191 stdcall DosDateTimeToFileTime(long long ptr) DosDateTimeToFileTime
192 stdcall DuplicateHandle(long long long ptr long long long) DuplicateHandle
193 stub EndUpdateResourceA
194 stub EndUpdateResourceW
195 stdcall EnterCriticalSection(ptr) EnterCriticalSection
196 stub EnumCalendarInfoA
197 stub EnumCalendarInfoW
198 stub EnumDateFormatsA
199 stub EnumDateFormatsW
200 stdcall EnumResourceLanguagesA(long str str ptr long) EnumResourceLanguages32A
201 stdcall EnumResourceLanguagesW(long wstr wstr ptr long) EnumResourceLanguages32W
202 stdcall EnumResourceNamesA(long str ptr long) EnumResourceNames32A
203 stdcall EnumResourceNamesW(long wstr ptr long) EnumResourceNames32W
204 stdcall EnumResourceTypesA(long ptr long) EnumResourceTypes32A
205 stdcall EnumResourceTypesW(long ptr long) EnumResourceTypes32W
206 stdcall EnumSystemCodePagesA(ptr long) EnumSystemCodePages32A
207 stdcall EnumSystemCodePagesW(ptr long) EnumSystemCodePages32W
208 stdcall EnumSystemLocalesA(ptr long) EnumSystemLocales32A
209 stdcall EnumSystemLocalesW(ptr long) EnumSystemLocales32W
210 stub EnumTimeFormatsA
211 stub EnumTimeFormatsW
212 stub EraseTape
213 stdcall EscapeCommFunction(long long) EscapeCommFunction32
214 stdcall ExitProcess(long) ExitProcess
215 stdcall ExitThread(long) ExitThread
216 stdcall ExpandEnvironmentStringsA(str ptr long) ExpandEnvironmentStrings32A
217 stdcall ExpandEnvironmentStringsW(wstr ptr long) ExpandEnvironmentStrings32W
218 stub FT_Exit0
219 stub FT_Exit12
220 stub FT_Exit16
221 stub FT_Exit20
222 stub FT_Exit24
223 stub FT_Exit28
224 stub FT_Exit32
225 stub FT_Exit36
227 stub FT_Exit40
228 stub FT_Exit44
229 stub FT_Exit48
226 stub FT_Exit4
230 stub FT_Exit52
231 stub FT_Exit56
232 stub FT_Exit8
233 stub FT_Prolog
234 stub FT_Thunk
235 stdcall FatalAppExitA(long str) FatalAppExit32A
236 stdcall FatalAppExitW(long wstr) FatalAppExit32W
237 stub FatalExit
238 stdcall FileTimeToDosDateTime(ptr ptr ptr) FileTimeToDosDateTime
239 stdcall FileTimeToLocalFileTime(ptr ptr) FileTimeToLocalFileTime
240 stdcall FileTimeToSystemTime(ptr ptr) FileTimeToSystemTime
241 stub FillConsoleOutputAttribute
242 stub FillConsoleOutputCharacterA
243 stub FillConsoleOutputCharacterW
244 stdcall FindAtomA(str) FindAtom32A
245 stdcall FindAtomW(wstr) FindAtom32W
247 stub FindCloseChangeNotification
246 stdcall FindClose(long) FindClose32
248 stdcall FindFirstChangeNotificationA(str long long) FindFirstChangeNotification32A
249 stub FindFirstChangeNotificationW
250 stdcall FindFirstFileA(str ptr) FindFirstFile32A
251 stdcall FindFirstFileW(wstr ptr) FindFirstFile32W
252 stdcall FindNextChangeNotification(long) FindNextChangeNotification
253 stdcall FindNextFileA(long ptr) FindNextFile32A
254 stdcall FindNextFileW(long ptr) FindNextFile32W
255 stdcall FindResourceA(long str str) FindResource32A
256 stdcall FindResourceExA(long str str long) FindResourceEx32A
257 stdcall FindResourceExW(long wstr wstr long) FindResourceEx32W
258 stdcall FindResourceW(long wstr wstr) FindResource32W
259 stdcall FlushConsoleInputBuffer(long) FlushConsoleInputBuffer
260 stdcall FlushFileBuffers(long) FlushFileBuffers
261 stub FlushInstructionCache
262 stdcall FlushViewOfFile(ptr long) FlushViewOfFile
263 stub FoldStringA
264 stub FoldStringW
265 stdcall FormatMessageA(long ptr long long ptr long ptr) FormatMessage32A
266 stdcall FormatMessageW(long ptr long long ptr long ptr) FormatMessage32W
267 stub FreeConsole
268 stdcall FreeEnvironmentStringsA(ptr) FreeEnvironmentStrings32A
269 stdcall FreeEnvironmentStringsW(ptr) FreeEnvironmentStrings32W
270 stub FreeLSCallback
272 stub FreeLibraryAndExitThread
271 stdcall FreeLibrary(long) FreeLibrary32
273 stdcall FreeResource(long) FreeResource32
274 stdcall FreeSLCallback(long) FreeSLCallback
275 stub GenerateConsoleCtrlEvent
276 stdcall GetACP() GetACP
277 stdcall GetAtomNameA(long ptr long) GetAtomName32A
278 stdcall GetAtomNameW(long ptr long) GetAtomName32W
279 stub GetBinaryType
280 stub GetBinaryTypeA
281 stub GetBinaryTypeW
282 stdcall GetCPInfo(long ptr) GetCPInfo
283 stub GetCommConfig
284 stdcall GetCommMask(long ptr) GetCommMask
285 stub GetCommModemStatus
286 stub GetCommProperties
287 stdcall GetCommState(long ptr) GetCommState32
288 stdcall GetCommTimeouts(long ptr) GetCommTimeouts
289 stdcall GetCommandLineA() GetCommandLine32A
290 stdcall GetCommandLineW() GetCommandLine32W
291 stub GetCompressedFileSizeA
292 stub GetCompressedFileSizeW
293 stdcall GetComputerNameA(ptr ptr) GetComputerName32A
294 stdcall GetComputerNameW(ptr ptr) GetComputerName32W
295 stdcall GetConsoleCP() GetConsoleCP
296 stdcall GetConsoleCursorInfo(long ptr) GetConsoleCursorInfo32
297 stdcall GetConsoleMode(long ptr) GetConsoleMode
298 stdcall GetConsoleOutputCP() GetConsoleOutputCP
299 stdcall GetConsoleScreenBufferInfo(long ptr) GetConsoleScreenBufferInfo
300 stdcall GetConsoleTitleA(ptr long) GetConsoleTitle32A
301 stdcall GetConsoleTitleW(ptr long) GetConsoleTitle32W
302 stub GetCurrencyFormatA
303 stub GetCurrencyFormatW
304 stdcall GetCurrentDirectoryA(long ptr) GetCurrentDirectory32A
305 stdcall GetCurrentDirectoryW(long ptr) GetCurrentDirectory32W
306 stdcall GetCurrentProcess() GetCurrentProcess
307 stdcall GetCurrentProcessId() GetCurrentProcessId
308 stdcall GetCurrentThread() GetCurrentThread
309 stdcall GetCurrentThreadId() GetCurrentThreadId
310 stdcall GetDateFormatA(long long ptr ptr ptr long) GetDateFormat32A
311 stub GetDateFormatW
312 stub GetDaylightFlag
313 stub GetDefaultCommConfigA
314 stub GetDefaultCommConfigW
315 stdcall GetDiskFreeSpaceA(str ptr ptr ptr ptr) GetDiskFreeSpace32A
316 stdcall GetDiskFreeSpaceW(wstr ptr ptr ptr ptr) GetDiskFreeSpace32W
317 stdcall GetDriveTypeA(str) GetDriveType32A
318 stdcall GetDriveTypeW(wstr) GetDriveType32W
320 stdcall GetEnvironmentStringsA() GetEnvironmentStrings32A
321 stdcall GetEnvironmentStringsW() GetEnvironmentStrings32W
319 stdcall GetEnvironmentStrings() GetEnvironmentStrings32A
322 stdcall GetEnvironmentVariableA(str ptr long) GetEnvironmentVariable32A
323 stdcall GetEnvironmentVariableW(wstr ptr long) GetEnvironmentVariable32W
324 stub GetErrorMode
325 stub GetExitCodeProcess
326 stdcall GetExitCodeThread(long ptr) GetExitCodeThread
327 stdcall GetFileAttributesA(str) GetFileAttributes32A
328 stdcall GetFileAttributesW(wstr) GetFileAttributes32W
329 stdcall GetFileInformationByHandle(long ptr) GetFileInformationByHandle
330 stdcall GetFileSize(long ptr) GetFileSize
331 stdcall GetFileTime(long ptr ptr ptr) GetFileTime
332 stdcall GetFileType(long) GetFileType
333 stdcall GetFullPathNameA(str long ptr ptr) GetFullPathName32A
334 stdcall GetFullPathNameW(wstr long ptr ptr) GetFullPathName32W
335 stub GetHandleContext
336 stub GetHandleInformation
337 stub GetLSCallbackTarget
338 stub GetLSCallbackTemplate
339 stdcall GetLargestConsoleWindowSize(long) GetLargestConsoleWindowSize
340 stdcall GetLastError() GetLastError
341 stdcall GetLocalTime(ptr) GetLocalTime
342 stdcall GetLocaleInfoA(long long ptr long) GetLocaleInfo32A
343 stdcall GetLocaleInfoW(long long ptr long) GetLocaleInfo32W
344 stdcall GetLogicalDriveStringsA(long ptr) GetLogicalDriveStrings32A
345 stdcall GetLogicalDriveStringsW(long ptr) GetLogicalDriveStrings32W
346 stdcall GetLogicalDrives() GetLogicalDrives
347 stub GetMailslotInfo
348 stdcall GetModuleFileNameA(long ptr long) GetModuleFileName32A
349 stdcall GetModuleFileNameW(long ptr long) GetModuleFileName32W
350 stdcall GetModuleHandleA(str) GetModuleHandle32A
351 stdcall GetModuleHandleW(wstr) GetModuleHandle32W
352 stub GetNamedPipeHandleStateA
353 stub GetNamedPipeHandleStateW
354 stub GetNamedPipeInfo
355 stub GetNumberFormatA
356 stub GetNumberFormatW
357 stdcall GetNumberOfConsoleInputEvents(long ptr) GetNumberOfConsoleInputEvents
358 stdcall GetNumberOfConsoleMouseButtons(long ptr) GetNumberOfConsoleMouseButtons
359 stdcall GetOEMCP() GetOEMCP
360 stub GetOverlappedResult
361 stdcall GetPriorityClass(long) GetPriorityClass
362 stdcall GetPrivateProfileIntA(str str long str) GetPrivateProfileInt32A
363 stdcall GetPrivateProfileIntW(wstr wstr long wstr) GetPrivateProfileInt32W
364 stdcall GetPrivateProfileSectionA(str str long str) GetPrivateProfileSection32A
365 stub GetPrivateProfileSectionNamesA
366 stub GetPrivateProfileSectionNamesW
367 stub GetPrivateProfileSectionW
368 stdcall GetPrivateProfileStringA(str str str ptr long str) GetPrivateProfileString32A
369 stdcall GetPrivateProfileStringW(wstr wstr wstr ptr long wstr) GetPrivateProfileString32W
370 stub GetPrivateProfileStructA
371 stub GetPrivateProfileStructW
372 stdcall GetProcAddress(long str) GetProcAddress32
373 stdcall GetProcessAffinityMask(long ptr ptr) GetProcessAffinityMask
374 stdcall GetProcessFlags(long) GetProcessFlags
375 stdcall GetProcessHeap() GetProcessHeap
376 stub GetProcessHeaps
377 stub GetProcessShutdownParameters
378 stdcall GetProcessTimes(long ptr ptr ptr ptr) GetProcessTimes
379 stdcall GetProcessVersion(long) GetProcessVersion
380 stdcall GetProcessWorkingSetSize(long ptr ptr) GetProcessWorkingSetSize
381 stub GetProductName
382 stdcall GetProfileIntA(str str long) GetProfileInt32A
383 stdcall GetProfileIntW(wstr wstr long) GetProfileInt32W
384 stdcall GetProfileSectionA(str str long) GetProfileSection32A
385 stub GetProfileSectionW
386 stdcall GetProfileStringA(str str str ptr long) GetProfileString32A
387 stdcall GetProfileStringW(wstr wstr wstr ptr long) GetProfileString32W
388 stub GetQueuedCompletionStatus
389 stub GetSLCallbackTarget
390 stub GetSLCallbackTemplate
391 stdcall GetShortPathNameA(str ptr long) GetShortPathName32A
392 stdcall GetShortPathNameW(wstr ptr long) GetShortPathName32W
393 stdcall GetStartupInfoA(ptr) GetStartupInfo32A
394 stdcall GetStartupInfoW(ptr) GetStartupInfo32W
395 stdcall GetStdHandle(long) GetStdHandle
396 stdcall GetStringTypeA(long long str long ptr) GetStringType32A
397 stdcall GetStringTypeExA(long long str long ptr) GetStringTypeEx32A
398 stdcall GetStringTypeExW(long long wstr long ptr) GetStringTypeEx32W
399 stdcall GetStringTypeW(long wstr long ptr) GetStringType32W
400 stdcall GetSystemDefaultLCID() GetSystemDefaultLCID
401 stdcall GetSystemDefaultLangID() GetSystemDefaultLangID
402 stdcall GetSystemDirectoryA(ptr long) GetSystemDirectory32A
403 stdcall GetSystemDirectoryW(ptr long) GetSystemDirectory32W
404 stdcall GetSystemInfo(ptr) GetSystemInfo
405 stdcall GetSystemPowerStatus(ptr) GetSystemPowerStatus
406 stdcall GetSystemTime(ptr) GetSystemTime
407 stub GetSystemTimeAdjustment
408 stub GetSystemTimeAsFileTime
409 stub GetTapeParameters
410 stub GetTapePosition
411 stub GetTapeStatus
412 stdcall GetTempFileNameA(str str long ptr) GetTempFileName32A
413 stdcall GetTempFileNameW(wstr wstr long ptr) GetTempFileName32W
414 stdcall GetTempPathA(long ptr) GetTempPath32A
415 stdcall GetTempPathW(long ptr) GetTempPath32W
416 stdcall GetThreadContext(long ptr) GetThreadContext
417 stdcall GetThreadLocale() GetThreadLocale
418 stdcall GetThreadPriority(long) GetThreadPriority
419 stdcall GetThreadSelectorEntry(long long ptr) GetThreadSelectorEntry
420 stub GetThreadTimes
421 stdcall GetTickCount() GetTickCount
422 stdcall GetTimeFormatA(long long ptr str ptr long) GetTimeFormat32A
423 stub GetTimeFormatW
424 stdcall GetTimeZoneInformation(ptr) GetTimeZoneInformation
425 stdcall GetUserDefaultLCID() GetUserDefaultLCID
426 stdcall GetUserDefaultLangID() GetUserDefaultLangID
427 stdcall GetVersion() GetVersion32
428 stdcall GetVersionExA(ptr) GetVersionEx32A
429 stdcall GetVersionExW(ptr) GetVersionEx32W
430 stdcall GetVolumeInformationA(str ptr long ptr ptr ptr ptr long) GetVolumeInformation32A
431 stdcall GetVolumeInformationW(wstr ptr long ptr ptr ptr ptr long) GetVolumeInformation32W
432 stdcall GetWindowsDirectoryA(ptr long) GetWindowsDirectory32A
433 stdcall GetWindowsDirectoryW(ptr long) GetWindowsDirectory32W
434 stdcall GlobalAddAtomA(str) GlobalAddAtom32A
435 stdcall GlobalAddAtomW(wstr) GlobalAddAtom32W
436 stdcall GlobalAlloc(long long) GlobalAlloc32
437 stdcall GlobalCompact(long) GlobalCompact32
438 stdcall GlobalDeleteAtom(long) GlobalDeleteAtom
439 stdcall GlobalFindAtomA(str) GlobalFindAtom32A
440 stdcall GlobalFindAtomW(wstr) GlobalFindAtom32W
441 stdcall GlobalFix(long) GlobalFix32
442 stdcall GlobalFlags(long) GlobalFlags32
443 stdcall GlobalFree(long) GlobalFree32
444 stdcall GlobalGetAtomNameA(long ptr long) GlobalGetAtomName32A
445 stdcall GlobalGetAtomNameW(long ptr long) GlobalGetAtomName32W
446 stdcall GlobalHandle(ptr) GlobalHandle32
447 stdcall GlobalLock(long) GlobalLock32
448 stdcall GlobalMemoryStatus(ptr) GlobalMemoryStatus
449 stdcall GlobalReAlloc(long long long) GlobalReAlloc32
450 stdcall GlobalSize(long) GlobalSize32
451 stdcall GlobalUnWire(long) GlobalUnWire32
452 stdcall GlobalUnfix(long) GlobalUnfix32
453 stdcall GlobalUnlock(long) GlobalUnlock32
454 stdcall GlobalWire(long) GlobalWire32
455 stub Heap32First
456 stub Heap32ListFirst
457 stub Heap32ListNext
458 stub Heap32Next
459 stdcall HeapAlloc(long long long) HeapAlloc
460 stdcall HeapCompact(long long) HeapCompact
461 stdcall HeapCreate(long long long) HeapCreate
462 stdcall HeapDestroy(long) HeapDestroy
463 stdcall HeapFree(long long ptr) HeapFree
464 stdcall HeapLock(long) HeapLock
465 stdcall HeapReAlloc(long long ptr long) HeapReAlloc
466 stub HeapSetFlags
467 stdcall HeapSize(long long ptr) HeapSize
468 stdcall HeapUnlock(long) HeapUnlock
469 stdcall HeapValidate(long long ptr) HeapValidate
470 stdcall HeapWalk(long ptr) HeapWalk
471 stub InitAtomTable
472 stdcall InitializeCriticalSection(ptr) InitializeCriticalSection
473 stdcall InterlockedDecrement(ptr) InterlockedDecrement
474 stdcall InterlockedExchange(ptr long) InterlockedExchange
475 stdcall InterlockedIncrement(ptr) InterlockedIncrement
476 stub InvalidateNLSCache
477 stdcall IsBadCodePtr(ptr) IsBadCodePtr32
478 stdcall IsBadHugeReadPtr(ptr long) IsBadHugeReadPtr32
479 stdcall IsBadHugeWritePtr(ptr long) IsBadHugeWritePtr32
480 stdcall IsBadReadPtr(ptr long) IsBadReadPtr32
481 stdcall IsBadStringPtrA(ptr long) IsBadStringPtr32A
482 stdcall IsBadStringPtrW(ptr long) IsBadStringPtr32W
483 stdcall IsBadWritePtr(ptr long) IsBadWritePtr32
484 stdcall IsDBCSLeadByte(long) IsDBCSLeadByte32
485 stdcall IsDBCSLeadByteEx(long long) IsDBCSLeadByteEx
486 stub IsLSCallback
487 stub IsSLCallback
488 stdcall IsValidCodePage(long) IsValidCodePage
489 stdcall IsValidLocale(long long) IsValidLocale
490 stub K32Thk1632Epilog
491 stub K32Thk1632Prolog
492 stdcall LCMapStringA(long long str long ptr long) LCMapString32A
493 stdcall LCMapStringW(long long wstr long ptr long) LCMapString32W
494 stdcall LeaveCriticalSection(ptr) LeaveCriticalSection
495 stdcall LoadLibraryA(str) LoadLibrary32A
496 stdcall LoadLibraryExA(str long long) LoadLibraryEx32A
497 stub LoadLibraryExW
498 stdcall LoadLibraryW(wstr) LoadLibrary32W
499 stdcall LoadModule(str ptr) LoadModule32
500 stdcall LoadResource(long long) LoadResource32
501 stdcall LocalAlloc(long long) LocalAlloc32
502 stdcall LocalCompact(long) LocalCompact32
503 stdcall LocalFileTimeToFileTime(ptr ptr) LocalFileTimeToFileTime
504 stdcall LocalFlags(long) LocalFlags32
505 stdcall LocalFree(long) LocalFree32
506 stdcall LocalHandle(ptr) LocalHandle32
507 stdcall LocalLock(long) LocalLock32
508 stdcall LocalReAlloc(long long long) LocalReAlloc32
509 stdcall LocalShrink(long long) LocalShrink32
510 stdcall LocalSize(long) LocalSize32
511 stdcall LocalUnlock(long) LocalUnlock32
512 stdcall LockFile(long long long long long) LockFile
513 stub LockFileEx
514 stdcall LockResource(long) LockResource32
515 stdcall MakeCriticalSectionGlobal(ptr) MakeCriticalSectionGlobal
516 stub MapHInstLS
517 stub MapHInstLS_PN
518 stub MapHInstSL
519 stub MapHInstSL_PN
520 stub MapHModuleLS
521 stub MapHModuleSL
522 stdcall MapLS(ptr) MapLS
523 stdcall MapSL(long) MapSL
524 stub MapSLFix
525 stdcall MapViewOfFile(long long long long long) MapViewOfFile
526 stdcall MapViewOfFileEx(long long long long long ptr) MapViewOfFileEx
527 stub Module32First
528 stub Module32Next
529 stdcall MoveFileA(str str) MoveFile32A
530 stdcall MoveFileExA(str str long) MoveFileEx32A
531 stdcall MoveFileExW(wstr wstr long) MoveFileEx32W
532 stdcall MoveFileW(wstr wstr) MoveFile32W
533 stdcall MulDiv(long long long) MulDiv32
534 stdcall MultiByteToWideChar(long long str long ptr long) MultiByteToWideChar
535 stub NotifyNLSUserCache
536 stdcall OpenEventA(long long str) OpenEvent32A
537 stdcall OpenEventW(long long wstr) OpenEvent32W
538 stdcall OpenFile(str ptr long) OpenFile32
539 stdcall OpenFileMappingA(long long str) OpenFileMapping32A
540 stdcall OpenFileMappingW(long long wstr) OpenFileMapping32W
541 stdcall OpenMutexA(long long str) OpenMutex32A
542 stdcall OpenMutexW(long long wstr) OpenMutex32W
543 stdcall OpenProcess(long long long) OpenProcess
544 stub OpenProfileUserMapping
545 stdcall OpenSemaphoreA(long long str) OpenSemaphore32A
546 stdcall OpenSemaphoreW(long long wstr) OpenSemaphore32W
547 stub OpenVxDHandle
548 stdcall OutputDebugStringA(str) OutputDebugString32A
549 stdcall OutputDebugStringW(wstr) OutputDebugString32W
550 stdcall PeekConsoleInputA(ptr ptr long ptr) PeekConsoleInput32A
551 stdcall PeekConsoleInputW(ptr ptr long ptr) PeekConsoleInput32W
552 stub PeekNamedPipe
553 stub PostQueuedCompletionStatus
554 stub PrepareTape
555 stub Process32First
556 stub Process32Next
557 stdcall PulseEvent(long) PulseEvent
558 stdcall PurgeComm(long long) PurgeComm
559 register QT_Thunk() QT_Thunk
560 stdcall QueryDosDeviceA(str ptr long) QueryDosDevice32A
561 stdcall QueryDosDeviceW(wstr ptr long) QueryDosDevice32W
562 stub QueryNumberOfEventLogRecords
563 stub QueryOldestEventLogRecord
564 stdcall QueryPerformanceCounter(ptr) QueryPerformanceCounter
565 stdcall QueryPerformanceFrequency(ptr) QueryPerformanceFrequency
566 stub QueueUserAPC
567 register RaiseException() EXC_RaiseException
568 stdcall ReadConsoleA(long ptr long ptr ptr) ReadConsole32A
569 stub ReadConsoleInputA
570 stub ReadConsoleInputW
571 stub ReadConsoleOutputA
572 stub ReadConsoleOutputAttribute
573 stub ReadConsoleOutputCharacterA
574 stub ReadConsoleOutputCharacterW
575 stub ReadConsoleOutputW
576 stdcall ReadConsoleW(long ptr long ptr ptr) ReadConsole32W
577 stdcall ReadFile(long ptr long ptr ptr) ReadFile
578 stdcall ReadFileEx(long ptr long ptr ptr) ReadFileEx
579 stdcall ReadProcessMemory(long ptr ptr long ptr) ReadProcessMemory
580 stdcall RegisterServiceProcess(long long) RegisterServiceProcess
581 stdcall ReinitializeCriticalSection(ptr) ReinitializeCriticalSection
582 stdcall ReleaseMutex(long) ReleaseMutex
583 stdcall ReleaseSemaphore(long long ptr) ReleaseSemaphore
584 stdcall RemoveDirectoryA(str) RemoveDirectory32A
585 stdcall RemoveDirectoryW(wstr) RemoveDirectory32W
586 stdcall ResetEvent(long) ResetEvent
587 stdcall ResumeThread(long) ResumeThread
588 stdcall RtlFillMemory(ptr long long) RtlFillMemory
589 stdcall RtlMoveMemory(ptr ptr long) RtlMoveMemory
590 register RtlUnwind() EXC_RtlUnwind
591 stdcall RtlZeroMemory(ptr long) RtlZeroMemory
592 register SMapLS() SMapLS
593 register SMapLS_IP_EBP_12() SMapLS_IP_EBP_12
594 register SMapLS_IP_EBP_16() SMapLS_IP_EBP_16
595 register SMapLS_IP_EBP_20() SMapLS_IP_EBP_20
596 register SMapLS_IP_EBP_24() SMapLS_IP_EBP_24
597 register SMapLS_IP_EBP_28() SMapLS_IP_EBP_28
598 register SMapLS_IP_EBP_32() SMapLS_IP_EBP_32
599 register SMapLS_IP_EBP_36() SMapLS_IP_EBP_36
600 register SMapLS_IP_EBP_40() SMapLS_IP_EBP_40
601 register SMapLS_IP_EBP_8() SMapLS_IP_EBP_8
602 register SUnMapLS() SUnMapLS
603 register SUnMapLS_IP_EBP_12() SUnMapLS_IP_EBP_12
604 register SUnMapLS_IP_EBP_16() SUnMapLS_IP_EBP_16
605 register SUnMapLS_IP_EBP_20() SUnMapLS_IP_EBP_20
606 register SUnMapLS_IP_EBP_24() SUnMapLS_IP_EBP_24
607 register SUnMapLS_IP_EBP_28() SUnMapLS_IP_EBP_28
608 register SUnMapLS_IP_EBP_32() SUnMapLS_IP_EBP_32
609 register SUnMapLS_IP_EBP_36() SUnMapLS_IP_EBP_36
610 register SUnMapLS_IP_EBP_40() SUnMapLS_IP_EBP_40
611 register SUnMapLS_IP_EBP_8() SUnMapLS_IP_EBP_8
612 stub ScrollConsoleScreenBufferA
613 stub ScrollConsoleScreenBufferW
614 stdcall SearchPathA(str str str long ptr ptr) SearchPath32A
615 stdcall SearchPathW(wstr wstr wstr long ptr ptr) SearchPath32W
616 stdcall SetCommBreak(long) SetCommBreak32
617 stub SetCommConfig
618 stdcall SetCommMask(long ptr) SetCommMask
619 stdcall SetCommState(long ptr) SetCommState32
620 stdcall SetCommTimeouts(long ptr) SetCommTimeouts
621 stub SetComputerNameA
622 stub SetComputerNameW
623 stub SetConsoleActiveScreenBuffer
624 stub SetConsoleCP
625 stdcall SetConsoleCtrlHandler(ptr long) SetConsoleCtrlHandler
626 stdcall SetConsoleCursorInfo(long ptr) SetConsoleCursorInfo32
627 stdcall SetConsoleCursorPosition(long long) SetConsoleCursorPosition
628 stdcall SetConsoleMode(long long) SetConsoleMode
629 stub SetConsoleOutputCP
630 stub SetConsoleScreenBufferSize
631 stub SetConsoleTextAttribute
632 stdcall SetConsoleTitleA(str) SetConsoleTitle32A
633 stdcall SetConsoleTitleW(wstr) SetConsoleTitle32W
634 stub SetConsoleWindowInfo
635 stdcall SetCurrentDirectoryA(str) SetCurrentDirectory32A
636 stdcall SetCurrentDirectoryW(wstr) SetCurrentDirectory32W
637 stub SetDaylightFlag
638 stub SetDefaultCommConfigA
639 stub SetDefaultCommConfigW
640 stdcall SetEndOfFile(long) SetEndOfFile
641 stdcall SetEnvironmentVariableA(str str) SetEnvironmentVariable32A
642 stdcall SetEnvironmentVariableW(wstr wstr) SetEnvironmentVariable32W
643 stdcall SetErrorMode(long) SetErrorMode32
644 stdcall SetEvent(long) SetEvent
645 stdcall SetFileApisToANSI() SetFileApisToANSI
646 stdcall SetFileApisToOEM() SetFileApisToOEM
647 stdcall SetFileAttributesA(str long) SetFileAttributes32A
648 stdcall SetFileAttributesW(wstr long) SetFileAttributes32W
649 stdcall SetFilePointer(long long ptr long) SetFilePointer
650 stdcall SetFileTime(long ptr ptr ptr) SetFileTime
651 stub SetHandleContext
652 stdcall SetHandleCount(long) SetHandleCount32
653 stub SetHandleInformation
654 stdcall SetLastError(long) SetLastError
655 stub SetLocalTime
656 stdcall SetLocaleInfoA(long long str) SetLocaleInfoA
657 stub SetLocaleInfoW
658 stub SetMailslotInfo
659 stub SetNamedPipeHandleState
660 stdcall SetPriorityClass(long long) SetPriorityClass
661 stdcall SetProcessShutdownParameters(long long) SetProcessShutdownParameters
662 stdcall SetProcessWorkingSetSize(long long long) SetProcessWorkingSetSize
663 stdcall SetStdHandle(long long) SetStdHandle
664 stdcall SetSystemPowerState(long long) SetSystemPowerState
665 stdcall SetSystemTime(ptr) SetSystemTime
666 stub SetSystemTimeAdjustment
667 stub SetTapeParameters
668 stub SetTapePosition
669 stdcall SetThreadAffinityMask(long long) SetThreadAffinityMask
670 stub SetThreadContext
671 stub SetThreadLocale
672 stdcall SetThreadPriority(long long) SetThreadPriority
673 stdcall SetTimeZoneInformation(ptr) SetTimeZoneInformation
674 stdcall SetUnhandledExceptionFilter(ptr) SetUnhandledExceptionFilter
675 stub SetVolumeLabelA
676 stub SetVolumeLabelW
677 stdcall SetupComm(long long long) SetupComm
678 stdcall SizeofResource(long long) SizeofResource32
679 stdcall Sleep(long) Sleep
680 stub SleepEx
681 stdcall SuspendThread(long) SuspendThread
682 stdcall SystemTimeToFileTime(ptr ptr) SystemTimeToFileTime
683 stub SystemTimeToTzSpecificLocalTime
684 stub TerminateProcess
685 stdcall TerminateThread(long long) TerminateThread
686 stub Thread32First
687 stub Thread32Next
688 stdcall ThunkConnect32(ptr str str str ptr ptr) ThunkConnect32
689 stdcall TlsAlloc() TlsAlloc
690 stub TlsAllocInternal
691 stdcall TlsFree(long) TlsFree
692 stub TlsFreeInternal
693 stdcall TlsGetValue(long) TlsGetValue
694 stdcall TlsSetValue(long ptr) TlsSetValue
695 stub Toolhelp32ReadProcessMemory
696 stub TransactNamedPipe
697 stdcall TransmitCommChar(long long) TransmitCommChar32
698 stdcall UTRegister(long str str str ptr ptr ptr) UTRegister
699 stdcall UTUnRegister(long) UTUnRegister
700 stdcall UnMapLS(long) UnMapLS
701 stub UnMapSLFixArray
702 stdcall UnhandledExceptionFilter(ptr) UnhandledExceptionFilter
703 stub UninitializeCriticalSection
704 stdcall UnlockFile(long long long long long) UnlockFile
705 stub UnlockFileEx
706 stdcall UnmapViewOfFile(ptr) UnmapViewOfFile
707 stub UpdateResourceA
708 stub UpdateResourceW
709 stub VerLanguageNameA
710 stub VerLanguageNameW
711 stdcall VirtualAlloc(ptr long long long) VirtualAlloc
712 stdcall VirtualFree(ptr long long) VirtualFree
713 stdcall VirtualLock(ptr long) VirtualLock
714 stdcall VirtualProtect(ptr long long ptr) VirtualProtect
715 stdcall VirtualProtectEx(long ptr long long ptr) VirtualProtectEx
716 stdcall VirtualQuery(ptr ptr long) VirtualQuery
717 stdcall VirtualQueryEx(long ptr ptr long) VirtualQueryEx
718 stdcall VirtualUnlock(ptr long) VirtualUnlock
719 stub WaitCommEvent
720 stub WaitForDebugEvent
721 stdcall WaitForMultipleObjects(long ptr long long) WaitForMultipleObjects
722 stdcall WaitForMultipleObjectsEx(long ptr long long long) WaitForMultipleObjectsEx
723 stdcall WaitForSingleObject(long long) WaitForSingleObject
724 stdcall WaitForSingleObjectEx(long long long) WaitForSingleObjectEx
725 stub WaitNamedPipeA
726 stub WaitNamedPipeW
727 stdcall WideCharToMultiByte(long long wstr long ptr long ptr ptr) WideCharToMultiByte
728 stdcall WinExec(str long) WinExec32
729 stdcall WriteConsoleA(long ptr long ptr ptr) WriteConsole32A
730 stub WriteConsoleInputA
731 stub WriteConsoleInputW
732 stub WriteConsoleOutputA
733 stub WriteConsoleOutputAttribute
734 stub WriteConsoleOutputCharacterA
735 stub WriteConsoleOutputCharacterW
736 stub WriteConsoleOutputW
737 stdcall WriteConsoleW(long ptr long ptr ptr) WriteConsole32W
738 stdcall WriteFile(long ptr long ptr ptr) WriteFile
739 stub WriteFileEx
740 stdcall WritePrivateProfileSectionA(str str str) WritePrivateProfileSection32A
741 stub WritePrivateProfileSectionW
742 stdcall WritePrivateProfileStringA(str str str str) WritePrivateProfileString32A
743 stdcall WritePrivateProfileStringW(wstr wstr wstr wstr) WritePrivateProfileString32W
744 stub WritePrivateProfileStructA
745 stub WritePrivateProfileStructW
746 stub WriteProcessMemory
747 stub WriteProfileSectionA
748 stub WriteProfileSectionW
749 stdcall WriteProfileStringA(str str str) WriteProfileString32A
750 stdcall WriteProfileStringW(wstr wstr wstr) WriteProfileString32W
751 stub WriteTapemark
752 stub _DebugOut
753 stub _DebugPrintf
754 stdcall _hread(long ptr long) _hread32
755 stdcall _hwrite(long ptr long) _hwrite32
756 stdcall _lclose(long) _lclose32
757 stdcall _lcreat(ptr long) _lcreat32
758 stdcall _llseek(long long long) _llseek32
759 stdcall _lopen(ptr long) _lopen32
760 stdcall _lread(long ptr long) _lread32
761 stdcall _lwrite(long ptr long) _lwrite32
762 stub dprintf
763 stdcall lstrcat(str str) lstrcat32A
764 stdcall lstrcatA(str str) lstrcat32A
765 stdcall lstrcatW(wstr wstr) lstrcat32W
766 stdcall lstrcmp(str str) lstrcmp32A
767 stdcall lstrcmpA(str str) lstrcmp32A
768 stdcall lstrcmpW(wstr wstr) lstrcmp32W
769 stdcall lstrcmpi(str str) lstrcmpi32A
770 stdcall lstrcmpiA(str str) lstrcmpi32A
771 stdcall lstrcmpiW(wstr wstr) lstrcmpi32W
772 stdcall lstrcpy(ptr str) lstrcpy32A
773 stdcall lstrcpyA(ptr str) lstrcpy32A
774 stdcall lstrcpyW(ptr wstr) lstrcpy32W
775 stdcall lstrcpyn(ptr str long) lstrcpyn32A
776 stdcall lstrcpynA(ptr str long) lstrcpyn32A
777 stdcall lstrcpynW(ptr wstr long) lstrcpyn32W
778 stdcall lstrlen(str) lstrlen32A
779 stdcall lstrlenA(str) lstrlen32A
780 stdcall lstrlenW(wstr) lstrlen32W
# 
# Functions exported by kernel32.dll in NT 3.51
# 
781 stub AddConsoleAliasA
782 stub AddConsoleAliasW
783 stub BaseAttachCompleteThunk
784 stub BasepDebugDump
785 stub CloseConsoleHandle
786 stub CmdBatNotification
787 stub ConsoleMenuControl
788 stub ConsoleSubst
789 stub CreateVirtualBuffer
790 stub ExitVDM
791 stub ExpungeConsoleCommandHistoryA
792 stub ExpungeConsoleCommandHistoryW
793 stub ExtendVirtualBuffer
794 stub FreeVirtualBuffer
795 stub GetConsoleAliasA
796 stub GetConsoleAliasExesA
797 stub GetConsoleAliasExesLengthA
798 stub GetConsoleAliasExesLengthW
799 stub GetConsoleAliasExesW
800 stub GetConsoleAliasW
801 stub GetConsoleAliasesA
802 stub GetConsoleAliasesLengthA
803 stub GetConsoleAliasesLengthW
804 stub GetConsoleAliasesW
805 stub GetConsoleCommandHistoryA
806 stub GetConsoleCommandHistoryLengthA
807 stub GetConsoleCommandHistoryLengthW
808 stub GetConsoleCommandHistoryW
811 stub GetConsoleDisplayMode
812 stub GetConsoleFontInfo
813 stub GetConsoleFontSize
814 stub GetConsoleHardwareState
815 stub GetConsoleInputWaitHandle
816 stub GetCurrentConsoleFont
817 stub GetNextVDMCommand
818 stub GetNumberOfConsoleFonts
819 stub GetVDMCurrentDirectories
820 stub HeapCreateTagsW
821 stub HeapExtend
822 stub HeapQueryTagW
824 stub HeapSummary
825 stub HeapUsage
826 stub InvalidateConsoleDIBits
827 stub IsDebuggerPresent
829 stub OpenConsoleW
830 stub QueryWin31IniFilesMappedToRegistry
831 stub RegisterConsoleVDM
832 stub RegisterWaitForInputIdle
833 stub RegisterWowBaseHandlers
834 stub RegisterWowExec
835 stub SetConsoleCommandHistoryMode
836 stub SetConsoleCursor
837 stub SetConsoleDisplayMode
838 stub SetConsoleFont
839 stub SetConsoleHardwareState
840 stub SetConsoleKeyShortcuts
841 stub SetConsoleMaximumWindowSize
842 stub SetConsoleMenuClose
843 stub SetConsoleNumberOfCommandsA
844 stub SetConsoleNumberOfCommandsW
845 stub SetConsolePalette
846 stub SetLastConsoleEventActive
847 stub SetVDMCurrentDirectories
848 stub ShowConsoleCursor
849 stub TrimVirtualBuffer
850 stub VDMConsoleOperation
851 stub VDMOperationStarted
852 stub VerifyConsoleIoHandle
853 stub VirtualBufferExceptionHandler
854 stub WriteConsoleInputVDMA
855 stub WriteConsoleInputVDMW

# NT 4.0 additions
856 stub CancelIo
857 stub CancelWaitableTimer
858 stub CopyFileExA
859 stub CopyFileExW
860 stub CreateFiber
861 stub CreateWaitableTimerA
862 stub CreateWaitableTimerW
863 stub DeleteFiber
864 stub DuplicateConsoleHandle
865 stub FindFirstFileExA
866 stub FindFirstFileExW
867 stub GetConsoleInputExeNameA
868 stub GetConsoleInputExeNameW
869 stub GetConsoleKeyboardLayoutNameA
870 stub GetConsoleKeyboardLayoutNameW
871 stdcall GetDiskFreeSpaceExA (str ptr ptr ptr) GetDiskFreeSpaceEx32A
873 stdcall GetDiskFreeSpaceExW (wstr ptr ptr ptr) GetDiskFreeSpaceEx32W
874 stub GetFileAttributesExA
875 stub GetFileAttributesExW
876 stub GetProcessPriorityBoost
877 stub GetThreadPriorityBoost
878 stub InterlockedCompareExchange
879 stub InterlockedExchangeAdd
880 stdcall IsProcessorFeaturePresent(long) IsProcessorFeaturePresent
881 stub OpenWaitableTimerA
882 stub OpenWaitableTimerW
883 stub ReadConsoleInputExA
884 stub ReadConsoleInputExW
885 stub ReadDirectoryChangesW
886 stub ReadFileScatter
887 stub SetConsoleIcon
888 stub SetConsoleInputExeNameA
889 stub SetConsoleInputExeNameW
890 stub SetProcessAffinityMask
891 stub SetProcessPriorityBoost
892 stub SetThreadIdealProcessor
893 stub SetThreadPriorityBoost
894 stub SetWaitableTimer
895 stub SignalObjectAndWait
896 stub SwitchToFiber
897 stub SwitchToThread
898 stdcall TryEnterCriticalSection(ptr) TryEnterCriticalSection
899 stub VirtualAllocEx
900 stub VirtualFreeEx
901 stub WriteFileGather

1346 stdcall PrivateLoadLibrary(str) PrivateLoadLibrary
1545 stdcall PrivateFreeLibrary(long) PrivateFreeLibrary

#1599 wrong ordinal (249 in Win32s's W32SCOMB.DLL) !
1599 stdcall Get16DLLAddress(long str) Get16DLLAddress
