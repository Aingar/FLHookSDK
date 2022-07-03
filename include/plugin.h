#pragma once

#include <functional>
#include <__generated.h>

#include "Enums.hpp"

#ifndef DLL
	#ifndef FLHOOK
	#define DLL __declspec(dllimport)
	#else
	#define DLL __declspec(dllexport)
	#endif
#endif

struct UserCommand
{
	std::wstring command;
	std::wstring usage;
	UserCmdProc proc;
	std::wstring description;
};

DLL UserCommand CreateUserCommand(const std::wstring& command, const std::wstring& usage, const UserCmdProc& proc, const std::wstring& description);

struct Timer
{
	std::function<void()> func;
	mstime intervalInSeconds;
	mstime lastTime = 0;
};

constexpr PluginMajorVersion CurrentMajorVersion = PluginMajorVersion::VERSION_04;
constexpr PluginMinorVersion CurrentMinorVersion = PluginMinorVersion::VERSION_00;

const std::wstring VersionInformation = std::to_wstring(static_cast<int>(CurrentMajorVersion)) + L"." + std::to_wstring(static_cast<int>(CurrentMinorVersion));

class PluginHook {
public:
    using FunctionType = void();
private:
    HookedCall targetFunction_;
    FunctionType *hookFunction_;
    HookStep step_;
    int priority_;
public:
    template <typename Func>
    PluginHook(HookedCall targetFunction, Func *hookFunction, HookStep step = HookStep::Before, int priority = 0)
        : targetFunction_(targetFunction), hookFunction_(reinterpret_cast<FunctionType*>(hookFunction)), step_(step), priority_(priority)
    {
	    switch (step) {
	    	case HookStep::Before:
				if (targetFunction == HookedCall::FLHook__LoadSettings)
					throw std::invalid_argument("Load settings can only be called HookStep::After.");
			    break;
	    	case HookStep::After:
			    break;
	    	case HookStep::Mid:
				throw std::invalid_argument("Mid hook step is not currently implemented");
	    	case HookStep::Count:
				throw std::invalid_argument("Count is not a valid hook step.");
		    default: ;
	    }
    }

    friend class PluginManager;
};

#define PluginCall(name, ...) (__stdcall * (name))(__VA_ARGS__)
// Inherit from this to define a IPC (Inter-Plugin Communication) class.
class DLL PluginCommunicator {
public:
    typedef void(__stdcall *EventSubscription)(int id, void *dataPack);
private:
    std::map<std::string, EventSubscription> listeners;
public:
    void Dispatch(int id, void *dataPack) const;
    void AddListener(std::string plugin, EventSubscription event);

    std::string plugin;
    PluginCommunicator(std::string plugin) : plugin(std::move(plugin)) {}

    static void ExportPluginCommunicator(PluginCommunicator *communicator);
    static PluginCommunicator *ImportPluginCommunicator(std::string plugin, PluginCommunicator::EventSubscription subscription = nullptr);
};

struct PluginInfo {
    DLL void versionMajor(PluginMajorVersion version);
	DLL void versionMinor(PluginMinorVersion version);
	DLL void name(const char* name);
	DLL void shortName(const char* shortName);
	DLL void mayPause(bool pause);
	DLL void mayUnload(bool unload);
	DLL void autoResetCode(bool reset);
	DLL void returnCode(ReturnCode* returnCode);
	DLL void addHook(const PluginHook& hook);
	DLL void commands(const std::vector<UserCommand>&);
	DLL void timers(const std::vector<Timer>&);

    #ifdef FLHOOK
		template<typename... Args>
		void addHook(Args&&... args)
		{
			addHook(PluginHook(std::forward<Args>(args)...));
		}

		PluginMajorVersion versionMajor_ = PluginMajorVersion::UNDEFINED;
		PluginMinorVersion versionMinor_ = PluginMinorVersion::UNDEFINED;
		std::string name_, shortName_;
		bool mayPause_ = false, mayUnload_ = false, resetCode_ = true;
		ReturnCode* returnCode_ = nullptr;
		std::list<PluginHook> hooks_;
	    std::vector<UserCommand> commands_;
	    std::vector<Timer> timers_;
    #else
		template<typename... Args>
		void emplaceHook(Args&&... args)
		{
			PluginHook ph(std::forward<Args>(args)...);
			addHook(ph);
		}
    #endif
};