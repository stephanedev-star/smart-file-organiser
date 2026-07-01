#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define FOLDER_DOCUMENTS  "Documents"
#define FOLDER_IMAGES     "Images"
#define FOLDER_VIDEOS     "Videos"
#define FOLDER_MUSIC      "Music"
#define FOLDER_PROGRAMS   "Programs"
#define FOLDER_ARCHIVES   "Archives"
#define FOLDER_OTHER      "Others"


#define LOG_FILE "organizer_log.txt"


void get_timestamp(char *buffer, int size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void log_action(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log == NULL) return;
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(log, "[%s] %s\n", timestamp, message);
    fclose(log);
}

void create_folder(const char *base_path, const char *folder_name) {
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s\\%s", base_path, folder_name);
    if (!CreateDirectoryA(full_path, NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            char msg[512];
            snprintf(msg, sizeof(msg), "ERROR: Could not create folder: %s", full_path);
            log_action(msg);
        }
    }
}

const char *get_target_folder(const char *extension) {

    char ext[32];
    int i;
    for (i = 0; extension[i] && i < 31; i++)
        ext[i] = (char)tolower((unsigned char)extension[i]);
    ext[i] = '\0';


    if (strcmp(ext, ".pdf")  == 0 || strcmp(ext, ".doc")  == 0 ||
        strcmp(ext, ".docx") == 0 || strcmp(ext, ".txt")  == 0 ||
        strcmp(ext, ".xls")  == 0 || strcmp(ext, ".xlsx") == 0 ||
        strcmp(ext, ".ppt")  == 0 || strcmp(ext, ".pptx") == 0 ||
        strcmp(ext, ".odt")  == 0 || strcmp(ext, ".csv")  == 0)
        return FOLDER_DOCUMENTS;


    if (strcmp(ext, ".jpg")  == 0 || strcmp(ext, ".jpeg") == 0 ||
        strcmp(ext, ".png")  == 0 || strcmp(ext, ".gif")  == 0 ||
        strcmp(ext, ".bmp")  == 0 || strcmp(ext, ".svg")  == 0 ||
        strcmp(ext, ".ico")  == 0 || strcmp(ext, ".webp") == 0 ||
        strcmp(ext, ".tiff") == 0 || strcmp(ext, ".raw")  == 0)
        return FOLDER_IMAGES;


    if (strcmp(ext, ".mp4")  == 0 || strcmp(ext, ".avi")  == 0 ||
        strcmp(ext, ".mkv")  == 0 || strcmp(ext, ".mov")  == 0 ||
        strcmp(ext, ".wmv")  == 0 || strcmp(ext, ".flv")  == 0 ||
        strcmp(ext, ".webm") == 0 || strcmp(ext, ".m4v")  == 0)
        return FOLDER_VIDEOS;


    if (strcmp(ext, ".mp3")  == 0 || strcmp(ext, ".wav")  == 0 ||
        strcmp(ext, ".flac") == 0 || strcmp(ext, ".aac")  == 0 ||
        strcmp(ext, ".ogg")  == 0 || strcmp(ext, ".wma")  == 0 ||
        strcmp(ext, ".m4a")  == 0)
        return FOLDER_MUSIC;


    if (strcmp(ext, ".exe")  == 0 || strcmp(ext, ".msi")  == 0 ||
        strcmp(ext, ".bat")  == 0 || strcmp(ext, ".cmd")  == 0 ||
        strcmp(ext, ".sh")   == 0 || strcmp(ext, ".dll")  == 0 ||
        strcmp(ext, ".apk")  == 0)
        return FOLDER_PROGRAMS;

    if (strcmp(ext, ".zip")  == 0 || strcmp(ext, ".rar")  == 0 ||
        strcmp(ext, ".7z")   == 0 || strcmp(ext, ".tar")  == 0 ||
        strcmp(ext, ".gz")   == 0 || strcmp(ext, ".iso")  == 0)
        return FOLDER_ARCHIVES;

    return FOLDER_OTHER;
}


void move_file(const char *source_path, const char *dest_folder, const char *filename) {
    char dest_path[MAX_PATH];
    snprintf(dest_path, sizeof(dest_path), "%s\\%s", dest_folder, filename);


    if (GetFileAttributesA(dest_path) != INVALID_FILE_ATTRIBUTES) {
        char name_only[MAX_PATH];
        char ext_only[64];
        char *dot = strrchr(filename, '.');
        if (dot) {
            strncpy(name_only, filename, dot - filename);
            name_only[dot - filename] = '\0';
            strcpy(ext_only, dot);
        } else {
            strcpy(name_only, filename);
            ext_only[0] = '\0';
        }
        int counter = 1;
        do {
            snprintf(dest_path, sizeof(dest_path), "%s\\%s_%d%s",
                     dest_folder, name_only, counter, ext_only);
            counter++;
        } while (GetFileAttributesA(dest_path) != INVALID_FILE_ATTRIBUTES);
    }

    if (MoveFileA(source_path, dest_path)) {
        char log_msg[MAX_PATH * 2 + 64];
        snprintf(log_msg, sizeof(log_msg), "MOVED: %s  -->  %s", source_path, dest_path);
        log_action(log_msg);
        printf("  [OK] Moved: %-40s --> %s\n", filename, dest_folder);
    } else {
        char log_msg[MAX_PATH * 2 + 64];
        snprintf(log_msg, sizeof(log_msg), "FAILED to move: %s (Error: %lu)", source_path, GetLastError());
        log_action(log_msg);
        printf("  [FAIL] Could not move: %s\n", filename);
    }
}

int organize_directory(const char *dir_path) {
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", dir_path);

    WIN32_FIND_DATAA find_data;
    HANDLE h_find = FindFirstFileA(search_path, &find_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        printf("ERROR: Cannot open directory: %s\n", dir_path);
        return -1;
    }


    create_folder(dir_path, FOLDER_DOCUMENTS);
    create_folder(dir_path, FOLDER_IMAGES);
    create_folder(dir_path, FOLDER_VIDEOS);
    create_folder(dir_path, FOLDER_MUSIC);
    create_folder(dir_path, FOLDER_PROGRAMS);
    create_folder(dir_path, FOLDER_ARCHIVES);
    create_folder(dir_path, FOLDER_OTHER);

    int files_moved = 0;

    do {
        const char *filename = find_data.cFileName;


        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) continue;
        if (strcmp(filename, LOG_FILE) == 0) continue;
        if (strcmp(filename, "smart_file_organizer.exe") == 0) continue;

        char *ext = strrchr(filename, '.');
        if (!ext) ext = "";


        const char *target_folder_name = get_target_folder(ext);

        char source_path[MAX_PATH];
        char target_folder_path[MAX_PATH];
        snprintf(source_path, sizeof(source_path), "%s\\%s", dir_path, filename);
        snprintf(target_folder_path, sizeof(target_folder_path), "%s\\%s", dir_path, target_folder_name);

        move_file(source_path, target_folder_path, filename);
        files_moved++;

    } while (FindNextFileA(h_find, &find_data));

    FindClose(h_find);
    return files_moved;
}


void print_header() {
    printf("============================================\n");
    printf("       SMART FILE ORGANIZER v1.0            \n");
    printf("   Author: DJIKEUCHIE NGAFOR CYRILLE        \n");
    printf("============================================\n\n");
}

int main(int argc, char *argv[]) {
    print_header();

    char dir_path[MAX_PATH];


    if (argc >= 2) {
        strncpy(dir_path, argv[1], MAX_PATH - 1);
        dir_path[MAX_PATH - 1] = '\0';
    } else {

        char *userprofile = getenv("USERPROFILE");
        if (userprofile) {
            snprintf(dir_path, sizeof(dir_path), "%s\\Downloads", userprofile);
        } else {
            GetCurrentDirectoryA(MAX_PATH, dir_path);
        }
    }

    printf("Target Directory: %s\n\n", dir_path);

    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    char start_msg[256];
    snprintf(start_msg, sizeof(start_msg), "===== Organization started for: %s =====", dir_path);
    log_action(start_msg);

    printf("Organizing files...\n");
    printf("-------------\n");

    int total = organize_directory(dir_path);

    printf("---------------\n");
    if (total < 0) {
        printf("Organization FAILED. Check the directory path.\n");
    } else if (total == 0) {
        printf("No files to organize. Directory is already clean.\n");
    } else {
        printf("\nDone! %d file(s) organized successfully.\n", total);
    }

    char end_msg[128];
    snprintf(end_msg, sizeof(end_msg), "===== Organization complete. Files moved: %d =====", total);
    log_action(end_msg);

    printf("\nLog saved to: %s\\%s\n", dir_path, LOG_FILE);
    printf("\nPress Enter to exit...\n");
    getchar();

    return 0;
}
