////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2012-2013 by authors (see AUTHORS.txt)                  //
//                                                                        //
//  This file is part of IOC.                                             //
//                                                                        //
//  IOC is free software; you can redistribute it and//or modify it under //
//  the terms of the IOC Public License(LGPL License) as published by     //
//  masol.lee.                                                            //
//                                                                        //
//  IOC is distributed in the hope that it will be useful,but WITHOUT     //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                  //
//                                                                        //
//  You should have received a copy of the IOC License along with this    //
//  program.  If not, see <http://www.masols.com/licenses>.               //
//                                                                        //
//  IOC website: http://www.masols.com                                    //
////////////////////////////////////////////////////////////////////////////


#include "config.h"
#include "runtime/runtime.h"
#include "llvm/Support/Atomic.h"
#include "llvm/Support/Threading.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/LLVMContext.h"

namespace ioc
{

Runtime::Runtime() : m_appModule(NULL)
{
	llvm::llvm_start_multithreaded();

    // Initialize targets first, so that --version shows registered targets.
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    // Initialize codegen and IR passes
    llvm::PassRegistry *Registry = llvm::PassRegistry::getPassRegistry();
    llvm::initializeCore(*Registry);
    llvm::initializeCodeGen(*Registry);
    llvm::initializeLoopStrengthReducePass(*Registry);
    llvm::initializeLowerIntrinsicsPass(*Registry);
    llvm::initializeUnreachableBlockElimPass(*Registry);
}

void
Runtime::pathName(const std::string &n)
{
    m_pathName = n;
    if(m_appModule)
    {
        m_appModule->setModuleIdentifier(m_pathName);
    }
}

llvm::Module*
Runtime::getModule()
{
    if(!m_appModule)
    {
        if(m_pathName.length() == 0)
        {
            m_pathName = "uname";
        }
        m_appModule = new llvm::Module(m_pathName,llvm::getGlobalContext());
    }
    return m_appModule;
}

Runtime::~Runtime()
{
	if(m_appModule)
    {
        delete m_appModule;
        m_appModule = NULL;
    }
	llvm::llvm_shutdown();
}

}
