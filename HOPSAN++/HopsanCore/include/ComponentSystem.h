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
//! @file   ComponentSystem.h
//! @author FluMeS
//! @date   2009-12-20
//!
//! @brief Contains the subsystem component class and connection assistant help class
//!
//$Id$

#ifndef COMPONENTSYSTEM_H
#define COMPONENTSYSTEM_H

#include "Component.h"

namespace tbb {
    class mutex;
}

namespace hopsan {

    class ConnectionAssistant
    {
    public:
        ConnectionAssistant(ComponentSystem *pComponentSystem);

        bool createNewNodeConnection(Port *pPort1, Port *pPort2, Node *&rpCreatedNode);
        bool mergeOrJoinNodeConnection(Port *pPort1, Port *pPort2, Node *&rpCreatedNode);
        bool deleteNodeConnection(Port *pPort1, Port *pPort2);
        bool splitNodeConnection(Port *pPort1, Port *pPort2);

        void determineWhereToStoreNodeAndStoreIt(Node* pNode);
        void clearSysPortNodeTypeIfEmpty(Port *pPort);

        bool ensureNotCrossConnecting(Port *pPort1, Port *pPort2);
        bool ensureSameNodeType(Port *pPort1, Port *pPort2);
        bool ensureConnectionOK(Node *pNode, Port *pPort1, Port *pPort2);

        void ifMultiportAddSubportAndSwapPtr(Port *&rpPort, Port *&rpOrignialPort);
        void ifMultiportCleanupAfterConnect(Port *pSubPort, Port *pMultiPort, const bool wasSucess);
        void ifMultiportPrepareForDisconnect(Port *&rpPort1, Port *&rpPort2, Port *&rpMultiPort1, Port *&rpMultiPort2);
        void ifMultiportCleanupAfterDisconnect(Port *&rpSubPort, Port *pMultiPort, const bool wasSucess);

    private:
        void recursivelySetNode(Port *pPort, Port *pParentPort, Node *pNode);
        Port* findMultiportSubportFromOtherPort(const Port *pMultiPort, Port *pOtherPort);
        ComponentSystem *mpComponentSystem; //The system to assist
    };

    class DLLIMPORTEXPORT SimulationHandler
    {
    public:
        enum SimulationErrorTypesT {NotRedy, InitFailed, SimuFailed, FiniFailed};

        //! @todo a doitall function
        //! @todo error enums
        bool initializeSystem(const double startT, const double stopT, ComponentSystem* pSystem);
        bool initializeSystem(const double startT, const double stopT, std::vector<ComponentSystem*> &rSystemVector);

        void simulateSystem(const double startT, const double stopT, const int nDesiredThreads, ComponentSystem* pSystem, bool noChanges=false);
        void simulateSystem(const double startT, const double stopT, const int nDesiredThreads, std::vector<ComponentSystem*> &rSystemVector, bool noChanges=false);

        void finalizeSystem(ComponentSystem* pSystem);
        void finalizeSystem(std::vector<ComponentSystem*> &rSystemVector);

    private:
        void simulateMultipleSystemsMultiThreaded(const double startT, const double stopT, const size_t nDesiredThreads, std::vector<ComponentSystem*> &rSystemVector, bool noChanges=false);
        void simulateMultipleSystems(const double startT, const double stopT, std::vector<ComponentSystem*> &rSystemVector);

        std::vector< std::vector<ComponentSystem*> > distributeSystems(const std::vector<ComponentSystem*> &rSystemVector, size_t nThreads);
        void sortSystemsByTotalMeasuredTime(std::vector<ComponentSystem*> &rSystemVector);

        std::vector< std::vector<ComponentSystem*> > mSplitSystemVector;
    };

    class DLLIMPORTEXPORT ComponentSystem :public Component
    {
        friend class ConnectionAssistant;

    public:
        //==========Public functions==========
        //Constructor - Destructor- Creator
        ComponentSystem();
        ~ComponentSystem();
        static Component* Creator(){ return new ComponentSystem(); }

        //Set the subsystem CQS type
        void setTypeCQS(CQSEnumT cqs_type, bool doOnlyLocalSet=false);
        bool changeSubComponentSystemTypeCQS(const std::string name, const CQSEnumT newType);
        void determineCQSType();

        //adding removing and renaming components
        void addComponents(std::vector<Component*> &rComponents);
        void addComponent(Component *pComponent);
        void renameSubComponent(std::string old_name, std::string new_name);
        void removeSubComponent(std::string name, bool doDelete=false);
        void removeSubComponent(Component *pComponent, bool doDelete=false);
        std::string reserveUniqueName(std::string desiredName);
        void unReserveUniqueName(std::string name);

        //System Parameter functions
        bool renameParameter(const std::string oldName, const std::string newName);

        //Handle system ports
        Port* addSystemPort(std::string portName);
        std::string renameSystemPort(const std::string oldname, const std::string newname);
        void deleteSystemPort(const std::string name);

        //Getting added components and component names
        Component* getSubComponentOrThisIfSysPort(std::string name);
        Component* getSubComponent(std::string name);
        ComponentSystem* getSubComponentSystem(std::string name);
        std::vector<std::string> getSubComponentNames();
        bool haveSubComponent(std::string name);

        //Connecting and disconnecting components
        bool connect(Port *pPort1, Port *pPort2);
        bool connect(std::string compname1, std::string portname1, std::string compname2, std::string portname2);
        bool disconnect(std::string compname1, std::string portname1, std::string compname2, std::string portname2);
        bool disconnect(Port *pPort1, Port *pPort2);

        // Convenience functions for enable and dissable data logging
        void setAllNodesDoLogData(const bool logornot);

        // Startvalue loading
        bool doesKeepStartValues();
        void setLoadStartValues(bool load);
        void loadStartValues();
        void loadStartValuesFromSimulation();

        // Initialize and simulate
        bool isSimulationOk();
        bool initialize(const double startT, const double stopT);
        void simulate(const double startT, const double stopT);
        void simulateMultiThreaded(const double startT, const double stopT, const size_t nDesiredThreads = 0, bool noChanges=false);
        void finalize();

#ifdef USETBB
        void simulateAndMeasureTime(const size_t steps = 1);
        double getTotalMeasuredTime();

        void sortComponentVectorsByMeasuredTime();
        void distributeCcomponents(std::vector< std::vector<Component*> > &rSplitCVector, size_t nThreads);
        void distributeQcomponents(std::vector< std::vector<Component*> > &rSplitQVector, size_t nThreads);
        void distributeSignalcomponents(std::vector< std::vector<Component*> > &rSplitSignalVector, size_t nThreads);
        void distributeNodePointers(std::vector< std::vector<Node*> > &rSplitNodeVector, size_t nThreads);
#endif

        void logAllNodes(const double time);

        //Set and get desired timestep
        void setDesiredTimestep(const double timestep);
        void setInheritTimestep(const bool inherit=true);
        bool doesInheritTimestep() const;
        double getDesiredTimeStep() const;

        //Get and set nLogSamples
        void setNumLogSamples(const size_t nLogSamples);
        size_t getNumLogSamples();

        //Stop a running init or simulation
        void stopSimulation();

#ifdef USETBB
        tbb::mutex *mpStopMutex;
#endif

        //System parameters
        bool setSystemParameter(const std::string name, const std::string value, const std::string type, const std::string description="", const std::string unit="", const bool force=false);
        Parameters &getSystemParameters();

    private:
        //==========Private functions==========
        //Time specific functions
        void setTimestep(const double timestep);
        void adjustTimestep(std::vector<Component*> componentPtrs);

        //log specific functions
        void preAllocateLogSpace(const double startT, const double stopT, const size_t nSamples = 2048);

        //Add and Remove sub nodes
        void addSubNode(Node* node_ptr);
        void removeSubNode(Node* node_ptr);

        //Add and Remove subcomponent ptrs from storage vectors
        void addSubComponentPtrToStorage(Component* pComponent);
        void removeSubComponentPtrFromStorage(Component* pComponent);

        void sortComponentVector(std::vector<Component*> &rOldSignalVector);
        bool componentVectorContains(std::vector<Component*> vector, Component *pComp);

        //UniqueName specific functions
        std::string determineUniquePortName(std::string portname);
        std::string determineUniqueComponentName(std::string name);

        //==========Private member variables==========
        typedef std::map<std::string, Component*> SubComponentMapT;
        typedef std::map<std::string, int> ReservedNamesT;
        SubComponentMapT mSubComponentMap;
        ReservedNamesT mReservedNames;
        std::vector<Component*> mComponentSignalptrs;
        std::vector<Component*> mComponentQptrs;
        std::vector<Component*> mComponentCptrs;
        std::vector<Component*> mComponentUndefinedptrs;

        std::vector<Node*> mSubNodePtrs;

        bool volatile mStopSimulation;

        bool mKeepStartValues;

        std::vector< std::vector<Component*> > mSplitCVector;                  //Create split vectors
        std::vector< std::vector<Component*> > mSplitQVector;
        std::vector< std::vector<Component*> > mSplitSignalVector;
        std::vector< std::vector<Node*> > mSplitNodeVector;

        std::vector<double *> mvTimePtrs;

        size_t mnLogSamples;

        //Finns i Component        Parameters *mSystemParameters;
    };
}


#endif // COMPONENTSYSTEM_H
