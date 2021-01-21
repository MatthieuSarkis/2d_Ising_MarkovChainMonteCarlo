#ifndef PATH_H_
#define PATH_H_

#include <iostream>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <sys/stat.h>
#include <regex>


using std::string;




struct Path
{

	static const char PATH_SEP;


	inline static string RemoveTrailingSlashes(const std::string& s)
	{
		size_t pos = s.length();
		for (auto it = s.crbegin(); it != s.crend(); ++it) 
		{
			if (*it != PATH_SEP) break;
			pos -= 1;
		}
		return s.substr(0, pos);
	}
	
    inline static string FindPathAfter(const string& path, const string& path1)
    {
        size_t pos = path.find(path1);
        pos += path1.length() + 1;
        return path.substr(pos);
    }

	inline static string Basename(const string& path)
	{
		size_t pos = path.find_last_of(PATH_SEP);
		return path.substr(pos + 1);
	}

	inline static string Basepath(const string& path)
	{
		size_t pos = path.find_last_of(PATH_SEP);

		if (pos == string::npos)
			return ".";
			
		return path.substr(0, pos);
	}


	inline static string Format(const string& path)
	{
		size_t pos = path.find_last_of(".");

		if (pos == string::npos)
			return "";

		return path.substr(pos);
	}

	inline static void PathSplit (const string& path, string& fpath, string& basename)
	{
		size_t pos = path.find_last_of(PATH_SEP);

		if (pos == string::npos)
		{
			fpath = ".";
			basename = path;
		}
		else
		{
			fpath = path.substr(0, pos);
			basename = path.substr(pos + 1);
		}
	}

	inline static void BasenameSplit (const string& path, string& fname, string& fmt)
	{
		const string basename = Basename(path);
		size_t pos = basename.find_last_of('.');

		if (pos == string::npos)
		{
			fname = basename;
			fmt = "";
		}
		else
		{
			fname = basename.substr(0, pos);
			fmt =  basename.substr(pos + 1);
		}
		
	}
	
	inline static int FileMode (const string& path)
	{
		struct stat info;
		
		if(stat( path.c_str(), &info ) != 0)
			return -1;		// the path does not exist
				
		else if(info.st_mode & S_IFREG)
			return 1;						// this is regular file mode
		else if(info.st_mode & S_IFDIR )
			return 2;						// this is dir mode
		else
			return 0;
	}


	inline static bool FileExists (const string& path)
	{
		struct stat info;
		return (stat (path.c_str(), &info) == 0);
	}

	bool static DirExists (const string& path)
	{
		struct stat info;

		if(stat( path.c_str(), &info ) != 0)
			return false;
		else if(info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}




	static void MakeDir(const string& path)
	{
		if (!DirExists(path))
		{
			#ifdef _WIN32
			mkdir(path.c_str());
			#else
			mkdir(path.c_str(), ACCESSPERMS); //  S_IRWXU | S_IRWXG | S_IRWXO
			#endif
		}
	}


	inline static string PathJoin(const string& dir, const string& d1)
	{
		if (dir != "")
			return dir + PATH_SEP + d1;
		else
			return d1;	
	}

	inline static string PathJoin(const string& dir, const string& d1, const string& d2)
	{
		string fpath = PathJoin(dir, d1);
		fpath = PathJoin(fpath, d2);
		return fpath;		
	}

	inline static string PathJoin(const std::vector<string>& dirs)
	{
        
		string fpath = "";
		for (const auto& d : dirs)
        {
            fpath = PathJoin(fpath, d);
            //std::cout << d << " " << fpath << std::endl;
        }        
		return fpath;
	}


	static void MakeDirRecursively(const string& dir)
	{
		const char tosearch = PATH_SEP;
		size_t pos = dir.find(tosearch);

		while(pos != std::string::npos)
		{
			string path = dir.substr(0, pos);
			MakeDir(path);
			pos = dir.find(tosearch, pos + 1);	// tosearch.size()
		}


		MakeDir(dir);
	}




	static void GetFiles(const string& dir, std::vector<string>& fpaths, int get_mode=0)
	{
		if (DirExists(dir))
		{
			DIR* dirp = opendir(dir.c_str());
			struct dirent* dp;
			while ((dp = readdir(dirp)) != NULL)
			{
				if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
				{
					string path = dir + PATH_SEP + dp->d_name;
										
					switch (get_mode)
					{
						case 0: 		// get all, files and dirs
							fpaths.push_back(path); 
							break;
							
						case 1:			//  just get files
							if (FileMode(path) == 1)
								fpaths.push_back(path); 
							break;
							
						case 2:			// just get dirs
							if (FileMode(path) == 2)
								fpaths.push_back(path); 
							break;
							
						default:
							std::cout << "# Error: the get_mode is " << get_mode << ", which should be 0, 1, 2" << std::endl;
							exit(0);							
					}
				}	
			}
			closedir(dirp);
		}
	}


	static void GetFiles(const std::vector<string>& dirs, std::vector<string>& fpaths, int get_mode=0)
	{
		for (const auto& dir : dirs)
		{
			GetFiles(dir, fpaths, get_mode);
		}

	}


	static void GetFilesRecursively(const string& idir, std::vector<string>& fpaths, bool just_leaf_files=true)
	{
		std::vector<string> current_paths;
		GetFiles(idir, current_paths, 0);
	
		
		std::vector<string> temp_dirs, temp_files;
		for (const auto& path : current_paths)
		{
			int mode = FileMode(path);
			
			if (mode == 1)
				temp_files.push_back(path);
			else if(mode == 2)
				temp_dirs.push_back(path);
		}
		
	
		
		if (just_leaf_files)
		{
			if (temp_dirs.size() == 0) // now we are on leaf
				fpaths.insert(fpaths.end(), temp_files.begin(), temp_files.end());
		}
		else
		{
			fpaths.insert(fpaths.end(), temp_files.begin(), temp_files.end());
		}
		
		current_paths.clear(); current_paths.shrink_to_fit();
		temp_files.clear(); temp_files.shrink_to_fit();
		
	
	
		// do again this job for each temp_dirs
		for (const auto& dir : temp_dirs)
		{
			GetFilesRecursively(dir, fpaths, just_leaf_files);
		}
	}
	
	static void GetFilesRecursively(const std::vector<string>& idirs, std::vector<string>& fpaths, bool just_leaf_files=true)
	{
		for (const auto& idir : idirs)
			GetFilesRecursively(idir, fpaths, just_leaf_files);
	}
	

	static void GetFilesRegextPattern(const std::vector<string>& fpaths_main, 
										std::vector<string>& fpaths,
										const string& regex_pattern)
	{
		std::regex reg(regex_pattern);
		for (const auto& path : fpaths_main)
		{
			//string fname = basename(path);
			if (std::regex_match(path, reg))
				fpaths.push_back(path);
		}	
	}
	
	static void GetFilesWithPattern(const std::vector<string>& fpaths_main, 
									std::vector<string>& fpaths,
									const std::vector<string>& patterns)
	{
		
		for (const auto& path : fpaths_main)
		{
			bool contain_all = true;
			
			for (const auto& pat : patterns)
			{
				if (path.find(pat) == std::string::npos)
				{
					contain_all = false;
					break;
				}
			}
			if (contain_all) fpaths.push_back(path);
		}
		
	}
	
	
	inline static size_t FileSize(const string& fpath)
	{
		struct stat fstat;
		stat(fpath.c_str(), &fstat);
		return fstat.st_size;
	}


	inline static size_t NumPacks(const string& fpath, size_t head_size, size_t pack_size)
	{
		struct stat fstat;
		stat(fpath.c_str(), &fstat);
		size_t size = fstat.st_size;


		//double d = (double)(size - head_size) / pack_size;
		size_t s = (size - head_size) / pack_size;


		// if the filesize is not compatilble
		/*if (fabs(d - s) > 1e-7 )
		{
            std::cout << "# Error in reading the size of file: d=" << d << " s=" << s << std::endl;
		}*/


		return s;

	}
	

};




#ifdef _WIN32
const char Path::PATH_SEP = '\\';
#else
const char Path::PATH_SEP = '/';
#endif




#endif
