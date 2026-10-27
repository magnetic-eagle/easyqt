#pragma once

#include <string>

namespace easyqt {
        template<typename Class>
        class Singleton {
                protected:
                        Singleton() {};
                        virtual ~Singleton() = default;
                        virtual void initImpl() {};
                        bool _inited = {false};
                        
                public:
                        bool init() {
                                if (!_inited) {
                                        initImpl();
                                        _inited = true;
                                        return false;
                                } else {
                                        return true;
                                }
                        };

                        bool inited() { return _inited; };
                
                        Singleton(Singleton const&) = delete;
                        Singleton& operator=(Singleton const&) = delete;
                        
                        static Class* instance() {
                                static Class instance = Class();
                                return &instance;
                        }
        };

        template<typename Class>
        class NamedSingleton: public Singleton<Class> {
                public:
                        const std::string& name() { return _name; };
                        static Class* instance() {
                                static Class* instance = new Class;
                                return instance;
                        }
               
                protected:
                        NamedSingleton(std::string name = ""): _name(name) {};
                        std::string _name = "";
        };
}
