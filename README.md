Smart File Organizer

A C program that automatically organizes files in a directory into categorized folders based on their file extensions. Designed to run every 2 hours on Windows startup using Task Scheduler.


Author

DJIKEUCHIE NGAFOR CYRILLE STEPHANE
Yaoundé International Business School — SWE Department


Features


Automatically scans a target directory (default: Downloads folder)
Sorts files into 7 categories based on extension:

Documents (.pdf, .docx, .txt, .xls, .pptx, .csv, ...)
Images (.jpg, .png, .gif, .bmp, .svg, ...)
Videos (.mp4, .avi, .mkv, .mov, ...)
Music (.mp3, .wav, .flac, .aac, ...)
Programs (.exe, .msi, .bat, .apk, ...)
Archives (.zip, .rar, .7z, .tar, ...)
Others (everything else)



Handles duplicate filenames automatically (adds number suffix)
Logs every action with timestamp to organizer_log.txt
Runs automatically every 2 hours via Windows Task Scheduler



Project Structure

Smart-File-Organizer/
│
├── smart_file_organizer.c     # Main source code
├── smart_file_organizer.exe   # Compiled executable (after build)
├── README.md                  # Project documentation
└── organizer_log.txt          # Auto-generated log file (after first run)

How to Compile

You need GCC for Windows (MinGW). If you don't have it, install it from https://www.mingw-w64.org.

Open Command Prompt and run:

bashgcc smart_file_organizer.c -o smart_file_organizer.exe


How to Run

Option 1: Run manually (organizes Downloads folder by default)

bashsmart_file_organizer.exe

Option 2: Specify a custom folder

bashsmart_file_organizer.exe "C:\Users\YourName\Desktop\TestFolder"


How to Set Up Automatic Execution (Every 2 Hours)

Using Windows Task Scheduler:


Press Win + S, search for Task Scheduler, open it.
Click Create Basic Task on the right panel.
Give it a name: Smart File Organizer
Click Next → Select When the computer starts → Click Next.
Select Start a Program → Click Next.
Browse and select smart_file_organizer.exe.
Click Finish.
Edit the task to repeat every 2 hours:

Right-click the task → Properties
Go to the Triggers tab → Edit the trigger
Check Repeat task every: → Set to 2 hours
Set for a duration of: → Indefinitely
Click OK → OK





The program will now run automatically every 2 hours after the computer starts.


File Categories and Extensions

FolderExtensionsDocuments.pdf .doc .docx .txt .xls .xlsx .ppt .pptx .odt .csvImages.jpg .jpeg .png .gif .bmp .svg .ico .webp .tiff .rawVideos.mp4 .avi .mkv .mov .wmv .flv .webm .m4vMusic.mp3 .wav .flac .aac .ogg .wma .m4aPrograms.exe .msi .bat .cmd .sh .dll .apkArchives.zip .rar .7z .tar .gz .isoOthersEverything else


Log File

Every time the program runs, it records all actions in organizer_log.txt in the target directory. Example:

[2026-06-30 14:00:01] ===== Organization started for: C:\Users\GIT\Downloads =====
[2026-06-30 14:00:01] MOVED: C:\Users\GIT\Downloads\report.pdf  -->  C:\Users\GIT\Downloads\Documents\report.pdf
[2026-06-30 14:00:01] MOVED: C:\Users\GIT\Downloads\song.mp3  -->  C:\Users\GIT\Downloads\Music\song.mp3
[2026-06-30 14:00:01] ===== Organization complete. Files moved: 2 =====


How It Works (Logic Flow)

START
  │
  ├── Read target directory
  ├── Create category folders (if not existing)
  │
  └── For each file in directory:
        │
        ├── Get file extension
        ├── Match extension to category
        ├── Move file to matching folder
        └── Log the action
  │
END


Requirements


Windows OS (7/8/10/11)
GCC / MinGW compiler
No external libraries needed (uses standard C + Windows API)



License

This project was developed as an internship task submission.
