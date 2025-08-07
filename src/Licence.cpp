/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%  Copyright @ 1999 ImageMagick Studio LLC, a non-profit organization         %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the License.  You may  %
%  obtain a copy of the License at                                            %
%                                                                             %
%    http://www.imagemagick.org/script/license.php                            %
%                                                                             %
%  Unless required by applicable law or agreed to in writing, software        %
%  distributed under the License is distributed on an "AS IS" BASIS,          %
%  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   %
%  See the License for the specific language governing permissions and        %
%  limitations under the License.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include "License.h"

void License::write(const Options &options,const Config &config,const wstring name)
{
  const auto targetDirectory=options.rootDirectory + L"Artifacts\\license\\";
  filesystem::create_directories(targetDirectory);

  wofstream licenseFile(targetDirectory + name + L".txt");
  for (const auto& license : config.licenses())
  {
    const auto sourceFileName=options.rootDirectory + config.directory() + license;
    wifstream sourceLicenseFile(sourceFileName);
    if (!sourceLicenseFile)
      throwException(L"Failed to open license file: " + sourceFileName);

    auto versionFileName=options.rootDirectory + config.directory() + L".ImageMagick\\ImageMagick.version.h";
    auto projectName=name;
    if (!filesystem::exists(versionFileName))
    {
      const auto configDirectory=sourceFileName.substr(0,sourceFileName.find_last_of(L"\\"));
      versionFileName=configDirectory + L"\\.ImageMagick\\ImageMagick.version.h";
      projectName=configDirectory.substr(configDirectory.find_last_of(L"\\") + 1);
    }

    wifstream versionFile(versionFileName);
    if (versionFile)
    {
      wstring
        line;

      getline(versionFile,line);
      getline(versionFile,line);
      if (!startsWith(line,L"#define DELEGATE_VERSION_STRING "))
        throwException(L"Invalid version file: " + versionFileName);
      line=line.substr(33,line.length() - 34);
      licenseFile << L"[ " << projectName << L" " << line << L" ]" << endl << endl;
    }
    else
    {
      licenseFile << L"[ " << projectName << L" ]" << endl << endl;
    }
    licenseFile << sourceLicenseFile.rdbuf() << endl;
  }
}

void License::writeNonWindowsLicenses(const Options &options)
{
  auto directory=L"Dependencies\\NonWindowsDependencies\\";
  if (!filesystem::exists(options.rootDirectory + directory))
    directory=L"NonWindowsDependencies\\";

  if (!filesystem::exists(options.rootDirectory + directory))
    return;

  for (const auto& entry : filesystem::directory_iterator(options.rootDirectory + directory))
  {
    if (!entry.is_directory())
      continue;

    auto name=entry.path().filename().wstring();
    auto projectDirectory=directory + name + L"\\";
    auto configFile=options.rootDirectory + projectDirectory + L".ImageMagick\\Config.txt";
    auto config=Config::load(name,projectDirectory,configFile);
    
    License::write(options,config,name);
  }
}
