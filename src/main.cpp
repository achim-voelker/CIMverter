#include <chrono>
#include "CIMObjectHandler.h"
#include <dirent.h>

/**
 * Get File size
 * @param filename
 * @return
 */
unsigned int filesize(const char *filename) {
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  long size = in.tellg();
  in.close();
  return size;
}
/**
 * Find all files in the folder
 * @param path
 * @return files
 */
std::vector<std::string> search_folder(const char *path) {

  struct dirent *ent = NULL;
  DIR *pDir;
  pDir = opendir(path);
  std::vector<std::string> files;
  
  while (NULL != (ent = readdir(pDir))) {

    std::string _path(path);
    std::string _dirName(ent->d_name);
    if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0 )
      continue;
    std::string file_path = _path + _dirName;
    files.push_back(file_path);
  }

  return files;

}

std::string modelica_filename("modelica_example");

int main(int argc, const char **argv) {

  long file_size;// File size
  long secs;// Time in seconds

  // Check for arguments
  if (argc <= 2) {
    if (strcmp(argv[1], "--help") == 0) {
      std::cout << "usage:" << std::endl;
      std::cout << "./CIM2Mod -f <file2.xml> <file2.xml>... [modelica_output_file_name]" << std::endl;
      std::cout << "./CIM2Mod -a <xml_directory/> [modelica_output_file_name]" << std::endl;
      exit(1);
    } else {
      std::cerr << "Too few arguments:" << std::endl;
      std::cout << "usage:" << std::endl;
      std::cout << "./CIM2Mod -f <file2.xml> <file2.xml>... [modelica_output_file_name]" << std::endl;
      std::cout << "./CIM2Mod -a <xml_directory/> [modelica_output_file_name]" << std::endl;
      exit(1);
    }
  }

  print_separator();

  CIMModel cimModel;

  if (argc > 2) {
    file_size = 0;
    if (strcmp(argv[1], "-f")==0) {
      for (int i = 2; i < argc - 1; i++) {
        std::cout << "CIM-XML file is:" << argv[i] << std::endl;
        file_size += filesize(argv[i]);
        cimModel.addCIMFile(argv[i]);
      }

    } else if (strcmp(argv[1], "-a")==0) {
      // Find all relevant files
      auto files = search_folder(argv[2]);

      for (auto f : files) {
        std::cout << "CIM-XML file is:" << f << std::endl;
        file_size += filesize(f.c_str());
        cimModel.addCIMFile(f);
      }

    } else {
      std::cout << "usage:" << std::endl;
      std::cout << "./CIM2Mod -f <file2.xml> <file2.xml>... [modelica_output_file_name]" << std::endl;
      std::cout << "./CIM2Mod -a <xml_directory/> [modelica_output_file_name]" << std::endl;
      exit(1);
    }

    modelica_filename = argv[argc - 1];

  }

  // Timer start
  std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
  start = std::chrono::high_resolution_clock::now();

  cimModel.parseFiles();// Parser begin!
  CIMObjectHandler ObjectHandler(std::move(cimModel.Objects));// r-value
  ObjectHandler.get_config();// Get configuration files
  ObjectHandler.ModelicaCodeGenerator(modelica_filename);

  // Timer stop
  stop = std::chrono::high_resolution_clock::now();
  secs = std::chrono::duration_cast<std::chrono::microseconds
  >(stop - start).count();

  std::cout << 1000*file_size/secs << "KByte/s" << std::endl;
  print_separator();
}
