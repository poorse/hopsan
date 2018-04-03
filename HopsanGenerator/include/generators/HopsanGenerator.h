/*-----------------------------------------------------------------------------

 Copyright 2017 Hopsan Group

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


 The full license is available in the file LICENSE.
 For details about the 'Hopsan Group' or information about Authors and
 Contributors see the HOPSANGROUP and AUTHORS files that are located in
 the Hopsan source code root directory.

-----------------------------------------------------------------------------*/

//!
//! @file   HopsanGenerator.h
//! @author Robert Braun <robert.braun@liu.se
//! @date   2012-01-08
//!
//! @brief Contains the Hopsan generator class
//!
//$Id$


#ifndef HOPSANGENERATOR_H
#define HOPSANGENERATOR_H

#ifdef _WIN32
#define LIBEXT ".dll"
#define LIBPREFIX ""
#else
#define LIBEXT ".so"
#define LIBPREFIX "lib"
#endif

#include <QString>
#include <QStringList>

#include "GeneratorUtilities.h"
#include "GeneratorTypes.h"

#include "hopsangenerator_win32dll.h"

// Forward declarations
namespace hopsan {
class ComponentSystem;
}


class HOPSANGENERATOR_DLLAPI HopsanGenerator
{
public:
    enum SolverT {NumericalIntegration, BilinearTransform};

    HopsanGenerator(const QString &hopsanInstallPath, const QString &compilerPath, const QString &tempPath="");
    virtual ~HopsanGenerator();

    void setOutputPath(const QString &path);

    QString getHopsanRootPath() const;
    QString getHopsanCoreIncludePath() const;
    QString getHopsanBinPath() const;
    QString getCompilerPath() const;

    void setQuiet(bool quiet);
    void printMessage(const QString &msg, const QString &color="Black") const;
    void printWarningMessage(const QString &msg) const;
    void printErrorMessage(const QString &msg) const;

    void compileFromComponentSpecification(const QString &outputFile, const ComponentSpecification &comp, const bool overwriteStartValues=false, const QString customSourceFile="");
    void generateNewLibrary(QString dstPath, QStringList hppFiles, QStringList cflags=QStringList(), QStringList lflags=QStringList());
    bool generateCafFile(QString &rPath, ComponentAppearanceSpecification &rCafSpec);

protected:
    virtual void handlePrintMessage(const QString &msg, const QString &color="Black") const;

    QString generateSourceCodefromComponentSpec(ComponentSpecification comp, bool overwriteStartValues=false) const;
    void generateOrUpdateComponentAppearanceFile(QString path, ComponentSpecification comp, QString sourceFile=QString());

    bool assertFilesExist(const QString &path, const QStringList &files) const;

    void callProcess(const QString &name, const QStringList &args, const QString workingDirectory=QString()) const;

    bool replaceInFile(const QString &filePath, const QStringList &before, const QStringList &after) const;

    bool copyHopsanCoreSourceFilesToDir(const QString &tgtPath) const;
    bool copyDefaultComponentCodeToDir(const QString &path) const;
    bool copyBoostIncludeFilesToDir(const QString &path) const;
    bool copyFile(const QString &source, const QString &target) const;
    bool copyDir(const QString &fromPath, const QString &toPath) const;
    void cleanUp(const QString &path, const QStringList &files, const QStringList &subDirs) const;
    void getNodeAndCqTypeFromInterfaceComponent(const QString &compType, QString &nodeType, QString &cqType);

    QString mHopsanRootPath;
    QString mHopsanCoreIncludePath;
    QString mHopsanBinPath;
    QString mCompilerPath;
    QString mTempPath;
    QString mOutputPath;

private:

    bool mShowMessages;

};


#endif // HOPSANGENERATOR_H
