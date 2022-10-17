#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <time.h>
#include <pwd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <stdbool.h>
#include <regex.h>
#define BUF_MAX 1024
#define CMD_DISPLAY_MAX (9 + 1)
#define USER_DISPLAY_MAX (10 + 1)
bool option[3] = {false};
char arg1[BUF_MAX], arg2[BUF_MAX], arg3[BUF_MAX];

struct pid_info_t {
    pid_t pid;
    char user[USER_DISPLAY_MAX];
    char cmdline[CMD_DISPLAY_MAX];
    char path[PATH_MAX];
    ssize_t parent_length;
};

void print_header() {
    printf("%-15s %-15s %-15s %-15s %-15s %-15s %s\n",
            "COMMAND", "PID", "USER", "FD", "TYPE", "NODE", "NAME");
}

void print_info(struct pid_info_t* info, char *FD, char *TYPE, char *inode, char *file) {
    // filter file infomation
    bool check_cond[3] = {false};
    if (option[0]) {
		regex_t reg;
		regcomp(&reg, arg1, REG_EXTENDED);
		const size_t nmatch = 1;
		regmatch_t pmatch[1];
		int status = regexec(&reg, info->cmdline, nmatch, pmatch, 0);
        if (status == REG_NOMATCH)
            check_cond[0] = true;
		regfree(&reg);
    }
    if (option[1]) {
        if (strcmp(TYPE, arg2) != 0)
            check_cond[1] = true;
    }
    if (option[2]) {
		regex_t reg;
		regcomp(&reg, arg3, REG_EXTENDED);
		const size_t nmatch = 1;
		regmatch_t pmatch[1];
		int status = regexec(&reg, file, nmatch, pmatch, 0);
        if (status == REG_NOMATCH)
            check_cond[2] = true;
		regfree(&reg);
    }
    // print the infomation of the opened file
    if (!check_cond[0] && !check_cond[1] && !check_cond[2]) {
        printf("%-15s %-15d %-15s %-15s %-15s %-15s %s\n",
			info->cmdline, info->pid, info->user, FD, TYPE, inode, file);
    }
}

// Get info. from specific file such as cwd, root and exe
void get_file(char *file, struct pid_info_t* info) {
	struct stat file_st;
	static ssize_t buff_size;
    static char buff[PATH_MAX];
    strncat(info->path, file, sizeof(info->path));
	if(strcmp(file, "cwd") == 0) {
		if ((buff_size = readlink(info->path, buff, sizeof(buff)-1)) < 0) {
			strncat(info->path, " (Permission denied)", sizeof(info->path));
			print_info(info, "cwd", "unknown", "", info->path);
		} else {
			char type[BUF_MAX];
			char inode[BUF_MAX];
			char path[BUF_MAX];
			strcpy(path, dirname(buff));
			buff[buff_size] = '\0';
			if (stat(info->path, &file_st) < 0) return;
			sprintf(inode, "%ld", file_st.st_ino);
			switch (file_st.st_mode & S_IFMT) {
				case S_IFDIR:	strcpy(type, "DIR");		break;
				case S_IFREG:	strcpy(type, "REG");		break;
				case S_IFCHR:	strcpy(type, "CHR");		break;
				case S_IFIFO:	strcpy(type, "FIFO");		break;
				case S_IFSOCK:	strcpy(type, "SOCK");		break;
				default:		strcpy(type, "unknown");	break;
			} 
			print_info(info, "cwd", type, inode, path);
		}
	} else if (strcmp(file, "root") == 0) {
		if ((buff_size = readlink(info->path, buff, sizeof(buff)-1)) < 0) {
			strncat(info->path, " (Permission denied)", sizeof(info->path));
			print_info(info, "rtd", "unknown", "", info->path);
		} else {
			char type[BUF_MAX];
			char inode[BUF_MAX];
			char path[BUF_MAX];
			strcpy(path, dirname(buff));
			buff[buff_size] = '\0';
			if (stat(info->path, &file_st) < 0) return;
			sprintf(inode, "%ld", file_st.st_ino);
			switch (file_st.st_mode & S_IFMT) {
				case S_IFDIR:	strcpy(type, "DIR");		break;
				case S_IFREG:	strcpy(type, "REG");		break;
				case S_IFCHR:	strcpy(type, "CHR");		break;
				case S_IFIFO:	strcpy(type, "FIFO");		break;
				case S_IFSOCK:	strcpy(type, "SOCK");		break;
				default:		strcpy(type, "unknown");	break;
			} 
			print_info(info, "rtd", type, inode, path);
		}
	} else if (strcmp(file, "exe") == 0) {
		if ((buff_size = readlink(info->path, buff, sizeof(buff)-1)) < 0) {
			strncat(info->path, " (Permission denied)", sizeof(info->path));
			print_info(info, "txt", "unknown", "", info->path);
		} else {
			char type[BUF_MAX];
			char inode[BUF_MAX];
			char path[BUF_MAX];
			strcpy(path, dirname(buff));
			buff[buff_size] = '\0';
			if (stat(info->path, &file_st) < 0) return;
			sprintf(inode, "%ld", file_st.st_ino);
			switch (file_st.st_mode & S_IFMT) {
				case S_IFDIR:	strcpy(type, "DIR");		break;
				case S_IFREG:	strcpy(type, "REG");		break;
				case S_IFCHR:	strcpy(type, "CHR");		break;
				case S_IFIFO:	strcpy(type, "FIFO");		break;
				case S_IFSOCK:	strcpy(type, "SOCK");		break;
				default:		strcpy(type, "unknown");	break;
			} 
			print_info(info, "txt", type, inode, path);
		}
	}	
	info->path[info->parent_length] = '\0';
}

// Get all info. from maps
void get_maps(struct pid_info_t* info) {
    strncat(info->path, "maps", sizeof(info->path));
	FILE *maps = fopen(info->path, "r");
	if (maps == NULL) { 
		info->path[info->parent_length] = '\0';
		return;
	}
	else {
		char inode[BUF_MAX], pre_inode[BUF_MAX], file[PATH_MAX], pre_file[PATH_MAX];
		char *token, *line_buff = NULL;
		ssize_t buff_size = 0;
		size_t offset;
		int break_condition = 0;
		while (getline(&line_buff, &buff_size, maps)) {
			if (break_condition >= 4) 
				break;
			token = strtok(line_buff, " \t\n");
			token = strtok(NULL, " \t\n");
			token = strtok(NULL, " \t\n");
			token = strtok(NULL, " \t\n");
			token = strtok(NULL, " \t\n");
			strncpy(inode, token, sizeof(inode));
			if (strcmp(inode, pre_inode) == 0)
				continue;
			// skip i-node number is zero
			if (strcmp(inode, "0") == 0) {
				break_condition++;
				continue;
			} else {
				break_condition = 0; }
			token = strtok(NULL, " \t\n");
			strncpy(file, token, sizeof(file));
			if (strcmp(file, pre_file) == 0)
            	continue;
			token = strtok(NULL, " \t\n");
			if (token == NULL)
				print_info(info, "mem", "REG", inode, file);
			else if (strcmp(token, "(deleted)") == 0)
				print_info(info, "DEL", "unknown", inode, file);
			strncpy(pre_inode, inode, sizeof(pre_inode));
			strncpy(pre_file, file, sizeof(pre_file));
		}
		free(line_buff);
		line_buff = NULL;
		fclose(maps);
	}
	info->path[info->parent_length] = '\0';
}

// Get all open file descriptors from fd
void get_fds(struct pid_info_t* info) {
    static char* fd_path = "fd/";
    strncat(info->path, fd_path, sizeof(info->path));
    int previous_length = info->parent_length;
    info->parent_length += strlen(fd_path);
    DIR *dir = opendir(info->path);
    if (dir == NULL) {
		strncat(info->path, " (Permission denied)", sizeof(info->path));
		print_info(info, "NOFD", "", "", info->path);
        info->parent_length = previous_length;
		info->path[info->parent_length] = '\0';
		return;    
	}
	struct dirent* de;
	while ((de = readdir(dir)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
		char descriptor[BUF_MAX];
		char type[BUF_MAX];
		char inode[BUF_MAX];
		char path[BUF_MAX];
		struct stat file_st;
		static ssize_t buff_size;
		static char buff[PATH_MAX];
		// Path name for file descriptor
		previous_length = strlen(info->path);
		strncat(info->path, de->d_name, sizeof(info->path));
		buff_size = readlink(info->path, buff, sizeof(buff)-1);
		strcpy(path, dirname(buff));
		buff[buff_size] = '\0';			
		// Inode for file descriptor
		if (stat(info->path, &file_st) < 0) return;	
		sprintf(inode, "%ld", file_st.st_ino);
		// Type for file descriptor
		switch (file_st.st_mode & S_IFMT) {
			case S_IFDIR:	strcpy(type, "DIR");		break;
			case S_IFREG:	strcpy(type, "REG");		break;
			case S_IFCHR:	strcpy(type, "CHR");		break;
			case S_IFIFO:	strcpy(type, "FIFO");		break;
			case S_IFSOCK:	strcpy(type, "SOCK");		break;
			default:		strcpy(type, "unknown");	break;
		} 
		// FD for file descriptor
		strcpy(descriptor, de->d_name);
		// if (access(path, R_OK) == 0 && access(path, W_OK) == 0)
		if ((file_st.st_mode & 0600) == 0600)
			strcat(descriptor, "u");
		else if ((file_st.st_mode & 0600) == S_IRUSR)
			strcat(descriptor, "r");
		else if ((file_st.st_mode & 0600) == S_IWUSR)
			strcat(descriptor, "w");
		// Path = pipe or socket
		if (strcmp(path, ".") == 0)
			// pipe:[node number] if it is a symbolic file to a pipe
			if (strcmp(type, "FIFO") == 0)
				sprintf(path, "pipe:[%ld]", file_st.st_ino);
			// Show socket:[node number] if it is a symbolic file to a socket
			if (strcmp(type, "SOCK") == 0)	
				sprintf(path, "socket:[%ld]", file_st.st_ino);
		print_info(info, descriptor, type, inode, path);
		info->path[previous_length] = '\0';
	}
    closedir(dir);
}

void lsof_dumpinfo(pid_t pid) {
    struct pid_info_t info;
    struct stat pidstat;
    struct passwd *pw;
    info.pid = pid;
    snprintf(info.path, sizeof(info.path), "/proc/%d/", pid);
    info.parent_length = strlen(info.path);
    // Get the UID by calling stat on the proc/pid directory.
    if (!stat(info.path, &pidstat)) {
        if (pw = getpwuid(pidstat.st_uid)) {
            strncpy(info.user, pw->pw_name, sizeof(info.user));
        } else {
            snprintf(info.user, USER_DISPLAY_MAX, "%d", (int)pidstat.st_uid);
        }
    } else {
        strcpy(info.user, "unknown");
    }
    // Get the name from the proc/pid/status directory
	char *line_buff = NULL;
	size_t line_buff_size = 0;
	strcat(info.path, "status");
	FILE *fd = fopen(info.path, "r");
	ssize_t line_size;
    if (!fd)
        return;
	line_size = getline(&line_buff, &line_buff_size, fd);
	char *token = strtok(line_buff, "\t\n");
	token = strtok(NULL, "\t\n");
	strncpy(info.cmdline, token, sizeof(info.cmdline));
	free(line_buff);
	line_buff = NULL;
	fclose(fd);
	info.path[info.parent_length] = '\0';
	// Read each of these symlinks
    get_file("cwd", &info);
	get_file("root", &info);
    get_file("exe", &info);
	get_maps(&info);
    get_fds(&info);
}

int main(int argc, char *argv[]) {
    long int pid = 0;
    char* endptr;
	DIR *dir = opendir("/proc");
    if (dir == NULL)
        return 0;
	// handle argument
    if (argc >= 3) {
        for (int i = 1; i < argc; i = i + 2) {
			if (strcmp(argv[i], "-c") == 0) {
                option[0] = true;
                strncpy(arg1, argv[i + 1], sizeof(arg1));
            } else if (strcmp(argv[i], "-t") == 0) {
                if (strcmp(argv[i + 1], "REG") != 0 &&
					strcmp(argv[i + 1], "CHR") != 0 && 
					strcmp(argv[i + 1], "DIR") != 0 && 
					strcmp(argv[i + 1], "FIFO") != 0 && 
					strcmp(argv[i + 1], "SOCK") != 0 && 
					strcmp(argv[i + 1], "unknown") != 0) {
                    fprintf(stderr, "Invalid TYPE option.\n");
                    return 0;
                } else {
                    option[1] = true;
                    strncpy(arg2, argv[i + 1], sizeof(arg2));
                }
            } else if (strcmp(argv[i], "-f") == 0) {
                option[2] = true;
                strncpy(arg3, argv[i + 1], sizeof(arg3));
            }
        }
    }       
    print_header();
	struct dirent* de;
	while ((de = readdir(dir))) {
		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;
		// Only inspect directories that are PID numbers
		pid = strtol(de->d_name, &endptr, 10);
		if (*endptr != '\0')
			continue;
		lsof_dumpinfo(pid);
	}
	closedir(dir);
    return 0;
}