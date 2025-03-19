/*
 * ----------------------------------------------------------------------------
 * (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 * The copyright of this Software is vested in the Crown
 * and the Software is the property of the Crown.
 * ----------------------------------------------------------------------------
 * SPDX-License-Identifier: Apache-2.0
 * ----------------------------------------------------------------------------
 */

#include "swa/Process.hh"
#include "swa/CommandLine.hh"
#include "swa/Duration.hh"
#include "swa/ProcessMonitor.hh"
#include "swa/ProgramError.hh"
#include "swa/Schedule.hh"
#include "swa/Timestamp.hh"
#include "swa/console.hh"

#include <dlfcn.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <sstream>

#include "libfile.hh"

namespace SWA {
    const char *const NAME_OPTION = "-name";
    const char *const COLD_START_OPTION = "-cold";

    const char *const PreInitSchedule = "-preinit";
    const char *const PostInitSchedule = "-postinit";

    using namespace std::literals::chrono_literals;

    void quitWithCleanup(int) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        SWA::Process::getInstance().requestShutdown();
    }

    Process::Process()
        : name(), shutdownRequested(false), ooa_strand(ioContext), work_guard(make_work_guard(ioContext)) {
        CommandLine::getInstance().registerOption(
            NamedOption(NAME_OPTION, "Name of the Process", false, "processName", true)
        );
        CommandLine::getInstance().registerOption(NamedOption(COLD_START_OPTION, "Cold Start", false));
        CommandLine::getInstance().registerOption(
            NamedOption(PreInitSchedule, "Schedules to run before initialisation", false, "scheduleFile", true, true)
        );
        CommandLine::getInstance().registerOption(
            NamedOption(PostInitSchedule, "Schedules to run after initialisation", false, "scheduleFile", true, true)
        );
    }

    Process &Process::getInstance() {
        static Process instance;
        return instance;
    }

    const Domain &Process::getDomain(int id) const {
        return domains[id];
    }

    Domain &Process::getDomain(int id) {
        return domains[id];
    }

    void Process::startNextSchedulePhase() {
        if (!shutdownRequested) {
            ProcessingThread thread("Run NextSchedulePhase");
            nextSchedulePhase();
            thread.completing();
            thread.complete();
        }
    }

    void Process::runScheduleFile(const std::string &fileName) {
        if (!shutdownRequested) {
            std::ostringstream fileStr;
            std::ifstream file(fileName.c_str());

            if (file >> fileStr.rdbuf()) {
                SWA::Schedule schedule(fileName, fileStr.str());

                if (schedule.isValid()) {
                    SWA::Process::getInstance().runSchedule(schedule);
                }
            } else {
                std::cerr << "Schedule file '" + fileName + "' not found. Ignored.\n";
            }
        }
    }

    void Process::runSchedule(const Schedule &schedule) {
        preSchedule();

        for (Schedule::Actions::const_iterator action = schedule.getActions().begin(),
                                               end = schedule.getActions().end();
             action != end && !shutdownRequested;
             ++action) {
            (*action)();
        }
        postSchedule();
    }

    void Process::runService(std::function<void()> service, const std::string &input) {
        ProcessingThread thread("Run Service");
        SWA::Device saveConsole(SWA::console());

        if (input.size() > 0) {
            SWA::console().setInputStream(std::make_shared<std::istringstream>(input));
        }
        service();
        if (input.size() > 0) {
            SWA::console() = saveConsole;
        }
        getEventQueue().processEvents();
        thread.completing();
        thread.complete();
    }

    void Process::idle(int timeout_secs) {
        ioContext.run_for(std::chrono::seconds(timeout_secs));
    }

    void Process::pause() {
        ProcessingThread thread("Pause");
        SWA::ProcessMonitor::getInstance().pauseRequested();
        getEventQueue().processEvents();
        thread.completing();
        thread.complete();
    }

    void Process::forceTerminate() {
        try {
            shutdown();
            exit(0);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    Domain &Process::registerDomain(const std::string &name) {
        DomainLookup::const_iterator it = domainLookup.find(name);

        if (it == domainLookup.end()) {
            int id = domains.size();
            domains.push_back(Domain(id, name, true));
            domainLookup.insert(DomainLookup::value_type(name, id));
            return domains.back();
        } else {
            // Already registered, so return the existing registration
            return domains[it->second];
        }
    }

    const Domain &Process::getDomain(const std::string &name) const {
        DomainLookup::const_iterator it = domainLookup.find(name);
        if (it == domainLookup.end())
            throw ProgramError("Invalid Domain Name: " + name);
        return domains[it->second];
    }

    Domain &Process::getDomain(const std::string &name) {
        DomainLookup::const_iterator it = domainLookup.find(name);
        if (it == domainLookup.end())
            throw ProgramError("Invalid Domain Name: " + name);
        return domains[it->second];
    }

    const std::string &Process::getName() const {
        // Lazy evaluation of name. Can't do it earlier, because
        // command line hasn't been parsed during construction.
        if (!name.length()) {
            if (CommandLine::getInstance().optionPresent(NAME_OPTION)) {
                name = CommandLine::getInstance().getOption(NAME_OPTION);
            } else {
                name = projectName;
                // No name supplied on the command line, so use the project name
                name = projectName;
            }
        }
        return name;
    }

    const CommandLine &Process::getCommandLine() const {
        return CommandLine::getInstance();
    }

    CommandLine &Process::getCommandLine() {
        return CommandLine::getInstance();
    }

    bool Process::coldStart() const {
        return CommandLine::getInstance().optionPresent(COLD_START_OPTION);
    }

    void Process::runStartup() const {
        starting();
    }

    void Process::endStartup() const {
        started();

        if (signal(SIGINT, SIG_IGN) != SIG_IGN)
            signal(SIGINT, quitWithCleanup);
        if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
            signal(SIGTERM, quitWithCleanup);
    }

    void Process::runSchedules() {
        preSchedules();

        std::vector<std::string> preInit = getCommandLine().getMultiOption(PreInitSchedule);
        std::for_each(preInit.begin(), preInit.end(), [this](const std::string &sched) {
            runScheduleFile(sched);
        });

        startNextSchedulePhase();

        std::vector<std::string> postInit = getCommandLine().getMultiOption(PostInitSchedule);
        std::for_each(postInit.begin(), postInit.end(), [this](const std::string &sched) {
            runScheduleFile(sched);
        });

        postSchedules();
    }

    void Process::endInitialisation() const {
        initialised();
    }

    boost::signals2::connection Process::registerStartupListener(const std::function<void()> &function) {
        return starting.connect(function);
    }

    boost::signals2::connection Process::registerInitialisingListener(const std::function<void()> &function) {
        return initialising.connect(function);
    }

    boost::signals2::connection Process::registerInitialisedListener(const std::function<void()> &function) {
        return initialised.connect(function);
    }

    boost::signals2::connection Process::registerStartedListener(const std::function<void()> &function) {
        return started.connect(function);
    }

    boost::signals2::connection Process::registerPreSchedulesListener(const std::function<void()> &function) {
        return preSchedules.connect(function);
    }

    boost::signals2::connection Process::registerPostSchedulesListener(const std::function<void()> &function) {
        return postSchedules.connect(function);
    }

    boost::signals2::connection Process::registerPreScheduleListener(const std::function<void()> &function) {
        return preSchedule.connect(function);
    }

    boost::signals2::connection Process::registerPostScheduleListener(const std::function<void()> &function) {
        return postSchedule.connect(function);
    }

    boost::signals2::connection Process::registerNextSchedulePhaseListener(const std::function<void()> &function) {
        return nextSchedulePhase.connect(function);
    }

    boost::signals2::connection
    Process::registerThreadStartedListener(const std::function<void(const std::string &)> &function) {
        return threadStarted.connect(function);
    }

    boost::signals2::connection Process::registerThreadCompletingListener(const std::function<void()> &function) {
        return threadCompleting.connect(function);
    }

    boost::signals2::connection Process::registerThreadCompletedListener(const std::function<void()> &function) {
        return threadCompleted.connect(function);
    }

    boost::signals2::connection Process::registerThreadAbortedListener(const std::function<void()> &function) {
        return threadAborted.connect(function);
    }

    boost::signals2::connection Process::registerShutdownListener(const std::function<void()> &function) {
        return shutdown.connect(function);
    }

    void Process::initialise() {
        ProcessingThread thread("Initialise Process");

        initialising();
        thread.completing();
        thread.complete();
    }

    void Process::mainLoop() {
        ioContext.run();
        shutdown();
    }

    Process::ProcessingThread::ProcessingThread(const std::string &name)
        : inProgress(true), name(name) {
        ProcessMonitor::getInstance().threadStarted();
        getInstance().threadStarted(name);
    }

    Process::ProcessingThread::~ProcessingThread() {
        if (inProgress) {
            getInstance().threadAborted();
            ProcessMonitor::getInstance().threadAborted();
        }
    }

    void Process::ProcessingThread::completing() {
        if (inProgress) {
            getInstance().threadCompleting();
            ProcessMonitor::getInstance().threadCompleting();
        }
    }

    void Process::ProcessingThread::complete() {
        if (inProgress) {
            getInstance().threadCompleted();
            ProcessMonitor::getInstance().threadCompleted();
            inProgress = false;
        }
    }

    void Process::ProcessingThread::abort() {
        if (inProgress) {
            getInstance().threadAborted();
            ProcessMonitor::getInstance().threadAborted();
            inProgress = false;
        }
    }

    void
    Process::loadDynamicLibraries(const std::string &libName, const std::string &interfaceSuffix, bool loadProjectLib) {

        // Set up the list of libraries to load
        std::set<std::string> requiredDomainLibs;

        for (SWA::Process::DomainList::const_iterator it = domains.begin(), end = domains.end(); it != end; ++it) {
            requiredDomainLibs.insert(
               libfile(it->getName() + (it->isInterface() ? interfaceSuffix : "") + "_" + libName)
            );
        }

        // Iterate over the list of domain libraries repeatedly until
        // either all have loaded or the errors from one pass to
        // the next are consistent
        std::set<std::string>::iterator domainLibIt = requiredDomainLibs.begin();
        std::string errorText;
        int errors = 0;
        int prevErrors = 0;

        while (domainLibIt != requiredDomainLibs.end()) {
            if (!dlopen(domainLibIt->c_str(), RTLD_NOW | RTLD_GLOBAL)) {
                errorText += std::string(dlerror()) + "\n";
                ++errors;
                ++domainLibIt;
            } else {
                // Successful load, so remove from the list
                requiredDomainLibs.erase(domainLibIt++);
            }

            if (domainLibIt == requiredDomainLibs.end()) {
                if (errors > 0 && errors != prevErrors) {
                    // A library didn't load, but at least one more than the last
                    // pass did, so try again
                    domainLibIt = requiredDomainLibs.begin();
                    prevErrors = errors;
                    errors = 0;
                    errorText.clear();
                }
            }
        }

        // If any of the required domain libs have not been loaded raise an error
        if (requiredDomainLibs.size()) {
            throw std::runtime_error(std::string("failed to load domain metadata library(s) : ") + errorText);
        }

        // Having loaded all the domain libraries, load the process library. This
        // needs to be done last as the process library will access the metadata of
        // the domain libraries in support of overriding the terminator services
        // required by the process.
        if (loadProjectLib && projectName.size()) {
            const std::string processLib = libfile(projectName + "_" + libName);
            if (!dlopen(processLib.c_str(), RTLD_NOW | RTLD_GLOBAL)) {
                throw std::runtime_error(
                    std::string("failed to load process metadata library ") + processLib + " : " +
                    std::string(dlerror()) + "\n"
                );
            }
        }
    }

    void Process::loadDynamicProjectLibrary(const std::string &libName) {
        const std::string processLib = libfile(projectName + "_" + libName);
        if (!dlopen(processLib.c_str(), RTLD_NOW | RTLD_GLOBAL)) {
            throw std::runtime_error(
                std::string("failed to load process metadata library ") + processLib + " : " + std::string(dlerror()) +
                "\n"
            );
        }
    }

} // namespace SWA
