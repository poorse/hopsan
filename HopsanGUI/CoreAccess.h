/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Björn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Linköping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   CoreAccess.h
//! @author Flumes <flumes@lists.iei.liu.se>
//! @date   2010-01-01
//!
//! @brief Contains the HopsanCore Qt API classes for communication with the HopsanCore
//!
//$Id$

#ifndef GUIROOTSYSTEM_H
#define GUIROOTSYSTEM_H

#include <QString>
#include <QVector>
#include <QPair>
#include <QStringList>

//Forward declarations of HopsanGUI classes
class LibraryWidget;
class SystemContainer;

//Forward declaration of HopsanCore classes
namespace hopsan {
class ComponentSystem;
class Port;
class SimulationHandler;
class CSVParser;
}


class CoreCSVParserAccess
{
public:
    CoreCSVParserAccess(QString file);
    bool isOk();
    int getNumberOfRows();
    int getNumberOfColumns();
    QVector<double> getColumn(int col);
private:
    hopsan::CSVParser *mpParser;
};


class CoreGeneratorAccess
{
public:
    enum FmuKind {ModelExchange, CoSimulation};
    enum TargetArchitectureT {x86, x64};
    bool generateFromModelica(QString path, bool showDialog=true, int solver=0, bool compile=false);
    bool generateFromCpp(QString hppFile, bool compile=false);
    bool generateFromFmu(QString path);
    bool generateToFmu(QString path, bool me, TargetArchitectureT architecture, SystemContainer *pSystem);
    bool generateToSimulink(QString path, SystemContainer *pSystem, bool disablePortLabels=false);
    bool generateToSimulinkCoSim(QString path, SystemContainer *pSystem, bool disablePortLabels=false, int compiler=0);
    bool generateToLabViewSIT(QString path, SystemContainer *pSystem);
    void generateLibrary(QString path, QStringList hppFiles);
    bool compileComponentLibrary(QString path, QString extraLibs="", bool showDialog=true);
};

class CoreLibraryAccess
{
public:
    bool hasComponent(const QString &rComponentName);
    bool loadComponentLib(const QString &rFileName);
    bool unLoadComponentLib(const QString &rFileName);
    bool reserveComponentTypeName(const QString &rTypeName);
    void getLoadedLibNames(QVector<QString> &rLibNames);
    void getLibraryContents(QString libPath, QStringList &rComponents, QStringList &rNodes);
    void getLibPathForComponent(QString typeName, QString &rLibPath);
};

class CoreMessagesAccess
{
public:
    unsigned int getNumberOfMessages();
    void getMessage(QString &rMessage, QString &rType, QString &rTag);
};

class CoreParameterData
{
public:
    CoreParameterData() : mIsDynamic(false) {}
    CoreParameterData(const QString name, const QString value, const QString type, const QString unit="", const QString desc="")
        : mName(name), mValue(value), mType(type), mUnit(unit), mDescription(desc), mIsDynamic(false) {}

    QString mName;
    QString mAlias;
    QString mValue;
    QString mType;
    QString mUnit;
    QString mDescription;
    QStringList mConditions;
    bool    mIsDynamic;
};


class CoreVariableData
{
public:
    CoreVariableData() {}
    CoreVariableData(const QString name, const QString value, const QString type, const QString unit="", const QString desc="")
        : mName(name), mValue(value), mType(type), mUnit(unit), mDescription(desc){}

    QString mName;
    QString mAlias;
    QString mValue;
    QString mType;
    QString mUnit;
    QString mDescription;
    QString mNodeDataVariableType;
};

class CoreVariameterDescription
{
public:
    CoreVariameterDescription() {}
    QString mName;
    QString mShortName;
    QString mPortName;
    QString mAlias;
    QString mDataType;
    QString mUnit;
    QString mDescription;
    QString mVariabelType;
    QStringList mConditions;
    int mVariameterType;
    int mVariabelId;
};

//Forward declaration
class CoreSimulationHandler;

class CoreSystemAccess
{
    friend class CoreSimulationHandler;
    friend class CoreGeneratorAccess;
public:
    enum PortTypeIndicatorT {InternalPortType, ActualPortType, ExternalPortType};

    CoreSystemAccess(QString name=QString(), CoreSystemAccess* pParentCoreSystemAccess=0);
    ~CoreSystemAccess();
    void deleteRootSystemPtr(); //!< @todo This is very strange, needed because core systems are deleted from parent if they are subsystems (not if root systems), this is the only way to safely delete the core object

    // Name and type functions
    //! @todo maybe we should use name="" (empty) to indicate root system instead, to cut down on the number of functions
    QString getSystemTypeCQS();
    QString getSubComponentTypeCQS(const QString componentName);

    // Commented by Peter, maybe should be used in the future
    // QString getSubComponentSubTypeName(const QString componentName) const;
    // void setSubComponentSubTypeName(const QString componentName, const QString subTypeName);

    QString setSystemName(QString name);
    QString getSystemName();
    QString renameSubComponent(QString componentName, QString name);

    // Parameters and start values
    bool hasParameter(const QString &rComponentName, const QString &rParameterName);
    QStringList getParameterNames(QString componentName);
    void getParameters(QString componentName, QVector<CoreParameterData> &rParameterDataVec);
    void getParameter(QString componentName, QString parameterName, CoreParameterData &rData);
    QString getParameterValue(QString componentName, QString parameterName);
    //void getStartValueDataNamesValuesAndUnits(QString componentName, QString portName, QVector<QString> &rNames, QVector<QString> &rStartDataValuesTxt, QVector<QString> &rUnits);
    bool setParameterValue(QString componentName, QString parameterName, QString value, bool force=0);

    void getVariameters(QString componentName, QVector<CoreVariameterDescription>& rVariameterDescriptions);

    // Alias functions
    bool setVariableAlias(QString compName, QString portName, QString varName, QString alias);
    QString getVariableAlias(QString compName, QString portName, QString varName);
    void setParameterAlias(QString compName, QString paramName, QString alias);
    void getFullVariableNameByAlias(QString alias, QString &rCompName, QString &rPortName, QString &rVarName);
    QStringList getAliasNames() const;



    // Port Functions
    //! @todo maybe need some get allport info function (instead of separate type nodetype description)
    QString getPortType(const QString componentName, const QString portName, const PortTypeIndicatorT portTypeIndicator=ActualPortType);
    QString getNodeType(QString componentName, QString portName);
    QString getPortDescription(QString componentName, QString portName);
    bool isPortConnected(QString componentName, QString portName);
    void setLoggingEnabled(const QString &componentName, const QString &portName, bool enable);
    bool isLoggingEnabled(const QString &componentName, const QString &portName);

    // Component creation and removal
    QString createComponent(QString type, QString name="");
    QString createSubSystem(QString name="");
    QString createConditionalSubSystem(QString name="");
    void removeSubComponent(QString componentName, bool doDelete);
    QString reserveUniqueName(QString desiredName);
    void unReserveUniqueName(QString name);

    // Component connection and disconnection
    bool connect(QString compname1, QString portname1, QString compname2, QString portname2);
    bool disconnect(QString compname1, QString portname1, QString compname2, QString portname2);

    // Simulation functions
    bool isSimulationOk();
    bool initialize(double mStartTime, double mFinishTime, int nSamples=2048);
    void simulate(double mStartTime, double mFinishTime, int nThreads=-1, bool modelHasNotChanged=false);
    void finalize();
    double getCurrentTime() const;
    void stop();
    bool writeNodeData(const QString compname, const QString portname, const QString dataname, double data);

    // System settings
    bool doesKeepStartValues();
    void setLoadStartValues(bool load);

    void setDesiredTimeStep(double timestep);
    void setDesiredTimeStep(QString compname, double timestep);
    void setInheritTimeStep(bool inherit);
    void setInheritTimeStep(QString compname, bool inherit);
    bool doesInheritTimeStep();
    bool doesInheritTimeStep(QString compname);

    double getDesiredTimeStep();
    size_t getNSamples();

    // System Port Functions
    void deleteSystemPort(QString portname);
    QString addSystemPort(QString portname);
    QString renameSystemPort(QString oldname, QString newname);

    // System Parameter Functions
    void loadParameterFile(QString fileName);
    QStringList getSystemParameterNames();
    void getSystemParameters(QVector<CoreParameterData> &rParameterDataVec);
    void getSystemParameter(const QString name, CoreParameterData &rParameterData);
    QString getSystemParameterValue(const QString name);
    bool setSystemParameter(const CoreParameterData &rParameter, bool force=false);
    bool setSystemParameterValue(QString name, QString value, bool force=false);
    bool hasSystemParameter(const QString name);
    bool renameSystemParameter(const QString oldName, const QString newName);
    void removeSystemParameter(const QString name);

    // Simulation results data retrieval
    void getVariableDescriptions(const QString compname, const QString portname, QVector<CoreVariableData> &rVarDescriptions);
    void getPlotDataNamesAndUnits(const QString compname, const QString portname, QVector<QString> &rNames, QVector<QString> &rUnits); //!< @deprecated
    std::vector<double> getTimeVector(QString componentName, QString portName);
    void getPlotData(const QString compname, const QString portname, const QString dataname, std::vector<double> *&rpTimeVector, QVector<double> &rData);
    std::vector<double> *getLogTimeData() const;
    bool havePlotData(const QString compname, const QString portname, const QString dataname);
    bool getLastNodeData(const QString compname, const QString portname, const QString dataname, double& rData) const;
    double *getNodeDataPtr(const QString compname, const QString portname, const QString dataname);

    //Time measurements
    void measureSimulationTime(QStringList &rComponentNames, QList<double> &rTimes, int nSteps=5);

    //Search path
    void addSearchPath(QString searchPath);

private:
    hopsan::ComponentSystem *getCoreSystemPtr();
    hopsan::ComponentSystem *getCoreSubSystemPtr(QString name);
    hopsan::Port* getCorePortPtr(QString componentName, QString portName) const;

    hopsan::ComponentSystem *mpCoreComponentSystem;
};


// Version functions
QString getHopsanCoreVersion();
QString getHopsanCoreCompiler();
QString getHopsanCoreArchitecture();
QString getHopsanCoreBuildTime();

class CoreSimulationHandler
{
public:
    //! @todo a doitall function
    bool initialize(const double startTime, const double stopTime, const double logStartTime, const int nLogSamples, CoreSystemAccess* pCoreSystemAccess);
    bool initialize(const double startTime, const double stopTime, const double logStartTime, const int nLogSamples, QVector<CoreSystemAccess*> &rvCoreSystemAccess);

    bool simulate(const double startTime, const double stopTime, const int nThreads, CoreSystemAccess* pCoreSystemAccess, bool modelHasNotChanged=false);
    bool simulate(const double startTime, const double stopTime, const int nThreads, QVector<CoreSystemAccess*> &rvCoreSystemAccess, bool modelHasNotChanged=false);

    void finalize(CoreSystemAccess* pCoreSystemAccess);
    void finalize(QVector<CoreSystemAccess*> &rvCoreSystemAccess);

    void runCoSimulation(CoreSystemAccess* pCoreSystemAccess);

private:


};

#ifdef USEZMQ
class RemoteCoreSimulationHandler
{
private:
    QString mRemoteServerAddress,   mRemoteServerPort;
    QString mHopsanDispatchAddress, mHopsanDispatchPort;

    bool mUseDispatch = false;


public:
    ~RemoteCoreSimulationHandler();

    void setHopsanServer(QString ip, QString port );
    void setHopsanDispatch( QString ip, QString port );

    void setUseDispatchServer(bool tf);
    bool usingDispatchServer() const;

    bool connect();
    void disconnect();

    bool loadModel(QString hmfModelFile);
    bool simulateModel();

    bool getCoreMessages(QVector<QString> &rTypes, QVector<QString> &rTags, QVector<QString> &rMessages, bool includeDebug=true);

    bool getLogData(std::vector<std::string> *pNames, std::vector<double> *pData);

    QString getLastError() const;
};
#endif


class NodeInfo
{
    public:
        NodeInfo(QString nodeType);
        static void getNodeTypes(QStringList &nodeTypes);

        QString niceName;
        QStringList qVariables;
        QStringList cVariables;
        QStringList variableLabels;
        QStringList shortNames;
        QList<size_t> varIdx;
        QString intensity;
        QString flow;
};


#endif // GUIROOTSYSTEM_H
