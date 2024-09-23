#include <iostream>

namespace DB
{
    struct IActionParameter
    {
        virtual std::string Address() const = 0;
    }; 
    struct System
    {
        void Execute( const IActionParameter& param )
        {
            std::cout << param.Address() << std::endl;
        }        
    };
}

namespace Application
{
    struct ActionParameter : DB::IActionParameter
    {
        std::string Address() const override { return "hello world"; };
    };
}

int main()
{
    DB::System db;
    Application::ActionParameter actionParameter;
    db.Execute( actionParameter );
    return 0;
}