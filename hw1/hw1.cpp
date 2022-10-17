#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cerrno>
#include <regex>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

bool filter[3] = {false};
string filtargu[3];

struct process{
	pid_t pid;
	string path;
	string dir;

	string cmd;
	string user;
	string type;
	string fd;
	string inode;
	string name;
};

void print_result(struct process file){

	bool match[3] = {true, true, true};

	if (filter[0])
	{
		regex reg(".*" + filtargu[0] + ".*");
		if (!regex_match(file.cmd, reg))
			match[0] = false;
		/*
		regex reg(filtargu[0]);
		if (!regex_search(file.cmd, reg))
			match[0] = false;
		*/
	}
	if (filter[1])
	{
		regex reg(".*" + filtargu[1] + ".*");
		if (!regex_match(file.type, reg))
			match[1] = false;
		/*
		regex reg(filtargu[1]);
		if (!regex_search(file.type, reg))
			match[1] = false;
		*/
	}
	if (filter[2])
	{
		regex reg(".*" + filtargu[2] + ".*");
		if (!regex_match(file.name, reg))
			match[2] = false;
		/*
		regex reg(filtargu[2]);
		if (!regex_search(file.name, reg))
			match[2] = false;
		*/
	}
	
	if (match[0] && match[1] && match[2])
	{
		if (file.dir == "cwd")
			file.fd = "cwd";
		else if (file.dir == "root")
			file.fd = "rtd";
		else if (file.dir == "exe")
			file.fd = "txt";
		
		while (file.name.find("(deleted)") != string::npos)
		{
			file.name = file.name.replace(file.name.find("(deleted)"), 9, "");
			file.fd = "DEL";
			file.type = "unknown";
		}
		
		printf("%-15s %3s %13s %4s %9s %18s %-64s\n", file.cmd.c_str()
													, to_string(file.pid).c_str()
													, file.user.c_str()
													, file.fd.c_str()
													, file.type.c_str()
													, file.inode.c_str()
													, file.name.c_str());
	}
	return;
}

void file_type(struct process file){
	
	struct stat file_info;

	if (!stat((file.path + file.dir).c_str(), &file_info))
	{
		file.inode = to_string(file_info.st_ino);
		if (S_ISDIR(file_info.st_mode))
			file.type = "DIR";
		else if (S_ISREG(file_info.st_mode))
			file.type = "REG";
		else if (S_ISCHR(file_info.st_mode))
			file.type = "CHR";
		else if (S_ISFIFO(file_info.st_mode))
			file.type = "FIFO";
		else if (S_ISSOCK(file_info.st_mode))
			file.type = "SOCK";
		else
			file.type = "unknown";
	}
	else
	{
		file.type = "unknown";
	   	file.inode = "";
	}

	int buf_size;
	char buf[PATH_MAX];
	if ((buf_size = readlink((file.path + file.dir).c_str(), buf, PATH_MAX)) == -1)
	{
		if (errno == ENOENT)
			return;
		file.name = file.path + file.dir + " (readlink: " + strerror(errno) + ")";
	}
	else
	{
		buf[buf_size] = '\0';
		file.name = buf;
	}
	
	print_result(file);
	return;
}

void file_maps(struct process file){
	
	struct stat file_info;
	
	if (!stat((file.path + file.dir).c_str(), &file_info))
	{
		file.inode = to_string(file_info.st_ino);
		if (S_ISDIR(file_info.st_mode))
			file.type = "DIR";
		else if (S_ISREG (file_info.st_mode))
			file.type = "REG";
		else if (S_ISCHR(file_info.st_mode))
			file.type = "CHR";
		else if (S_ISFIFO(file_info.st_mode))
			file.type = "FIFO";
		else if (S_ISSOCK(file_info.st_mode))
			file.type = "SOCK";
		else
			file.type = "unknown";
	}

	// Avoid printing one unnecessary line.
	if (!stat((file.path + "exe").c_str(), &file_info))
		file.inode = to_string(file_info.st_ino);

	file.fd = "mem";
	
	string buf;
	string addr, permission, offset, device, inode, pathname;
	ifstream mapsfile(file.path + "maps");
	stringstream ss;
	if (mapsfile)
	{
		while(getline(mapsfile, buf))
		{
			ss << buf;
			ss >> addr >> permission >> offset >> device >> inode >> pathname;
			ss.str("");
        	ss.clear();

			if (offset == "00000000" && inode != "0" && device != "00:00" && inode != file.inode)
			{
				file.inode = inode;
				file.name = pathname;
				print_result(file);
			}
		}
	}
	mapsfile.close();

	return;
}

void file_fd(struct process file){

	DIR *dp = opendir((file.path + file.dir).c_str());
	dirent *dirp;
	if (errno == EACCES)
	{
		file.type = "";
		file.inode = "";
		file.name = file.path + file.dir + " (opendir: " + strerror(errno) + ")";
		file.fd = "NOFD";
		print_result(file);
	}
	else
	{
		while((dirp = readdir(dp)) != NULL)
		{
			if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
				continue;
			
			int buf_size;
			char buf[PATH_MAX];
			if ((buf_size = readlink((file.path + file.dir + "/" + dirp->d_name).c_str(), buf, PATH_MAX)) != -1)
			{
				buf[buf_size] = '\0';
				file.name = buf;

				struct stat file_info;
				if (!stat((file.path + file.dir + "/" + dirp->d_name).c_str(), &file_info))
				{
					file.inode = to_string(file_info.st_ino);
					if (S_ISDIR(file_info.st_mode))
						file.type = "DIR";
					else if (S_ISREG(file_info.st_mode))
						file.type = "REG";
					else if (S_ISCHR(file_info.st_mode))
						file.type = "CHR";
					else if (S_ISFIFO(file_info.st_mode))
						file.type = "FIFO";
					else if (S_ISSOCK(file_info.st_mode))
						file.type = "SOCK";
					else
						file.type = "unknown";
				}
				else
				{
					file.type = "unknown";
					file.inode = "";
				}

				if (!lstat((file.path + file.dir + "/" + dirp->d_name).c_str(), &file_info))
				{
					file.fd = dirp->d_name;
					if ((file_info.st_mode & 0600) == 0600)
						file.fd.append("u");
					else if ((file_info.st_mode & S_IRUSR) == S_IRUSR)
						file.fd.append("r");
					else if ((file_info.st_mode & S_IWUSR) == S_IWUSR)
						file.fd.append("w");
				}
				print_result(file);
			}
		}
	}
	closedir(dp);
	return;
}

void lsof(struct process file){
	
	struct stat file_info;
	file.path = "/proc/" + to_string(file.pid) + "/";
	
	struct passwd *pwd;
	if (!stat(file.path.c_str(), &file_info))
	{
		if ((pwd = getpwuid(file_info.st_uid)) != NULL)
		{
			file.user = pwd->pw_name;
		}
	}

	string command;
	ifstream statfile;
	statfile.open(file.path + "stat");
	if (!statfile)
	{
		cerr << "Can't open the stat file!\n";
		exit(1);
	}
	statfile >> command >> command;
	statfile.close();
	file.cmd = command.substr(1, command.length() - 2);

	file.dir = "cwd";
	file_type(file);
	file.dir = "root";
	file_type(file);
	file.dir = "exe";
	file_type(file);
	file.dir = "maps";
	file_maps(file);
	file.dir = "fd";
	file_fd(file);

	return;
}

int main(int argc, char* argv[]){

	int opt, i = 0;

	while((opt = getopt(argc, argv, "c:t:f:?")) != EOF)
	{
		switch(opt)
		{
			case 'c':
			{
				filter[0] = true;
				filtargu[0] = argv[2 * i + 2];
				break;
			}
			case 't':
			{
				filter[1] = true;
				if (strcmp(argv[2 * i + 2], "REG") && strcmp(argv[2 * i + 2], "CHR") && strcmp(argv[2 * i + 2], "DIR") && strcmp(argv[2 * i + 2], "FIFO") && strcmp(argv[2 * i + 2], "SOCK"))
				{
					printf("Invalid TYPE option.\n");
					exit(1);
				}
				filtargu[1] = argv[2 * i + 2];
				break;
			}
			case 'f':
			{
				filter[2] = true;
				filtargu[2] = argv[2 * i + 2];
				break;
			}
		}
		i++;
	}
	
	printf("%-15s %3s %13s %4s %9s %18s %-64s\n", "COMMAND"
												, "PID"
												, "USER"
												, "FD"
												, "TYPE"
												, "NODE"
												, "NAME");
	
	DIR *dp = opendir("/proc/");
	char *ptr;
	dirent *dirp;
	struct process file;

	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;
		file.pid = strtol(dirp->d_name, &ptr, 10);
		if (file.pid == 0)
			continue;
		/*
		file.inode = to_string(dirp->d_ino);
		if (dirp->d_type  == DT_DIR)
			file.type = "DIR";
		else if (dirp->d_type == DT_REG)
			file.type = "REG";
		else if (dirp->d_type == DT_CHR)
			file.type = "CHR";
		else if (dirp->d_type == DT_FIFO)
			file.type = "FIFO";
		else if (dirp->d_type == DT_SOCK)
			file.type = "SOCK";
		else
			file.type = "unknown";
		*/
		lsof(file);
	}
	closedir(dp);
	return 0;
}