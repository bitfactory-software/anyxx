#include <iostream>
#include <string>
#include <any>
#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <typeindex>

namespace library
{
    template< typename R, typename ANY, typename... ARGS >
    class declare_any_dispatch
    {
		using erased_function_t = std::function< R( ANY, ARGS... ) >;
	public:
		std::map< std::type_index, erased_function_t > dispatchTable_;
        erased_function_t fallback_ = []( ANY dispatch, ARGS... args )->R
        {
			throw std::runtime_error( std::string( dispatch.type().name() ) + ": No registered method." );
        };
	public:
		void define_erased( const std::type_info& register_type_info, const erased_function_t& f )
		{
			auto entry = dispatchTable_.find( register_type_info );
			if( entry != dispatchTable_.end() )
				throw std::runtime_error( "Method for type already registered." );
			dispatchTable_[ register_type_info ] = f;
		}
		auto define_fallback( const erased_function_t& fallback )
		{
            fallback_ = fallback;
		}
        template< typename DISPATCH >
		auto define( const auto& f )
		{
			return define_erased( typeid( DISPATCH ), [ f ]( ANY dispatch, ARGS... args )->R
			{
                auto unerased = std::any_cast< DISPATCH >( &dispatch );   
				return f( unerased, std::forward< ARGS >( args )... );
			});
		}
		R operator()( ANY dispatch, ARGS... args ) const
		{
			auto entry = dispatchTable_.find( dispatch.type() );
			if( entry == dispatchTable_.end() )
				return fallback_( dispatch, args... );
			return entry->second( dispatch, args... );
		}	
		erased_function_t is_defined( const std::type_info& type_info )
		{
			if( auto entry = dispatchTable_.find( type_info ); entry != dispatchTable_.end() )
				return entry->second;
			return nullptr_t{};
		}
		template< typename C >
		erased_function_t is_defined()
		{
			return is_defined( typeid( C ) );
		}
    };
}

namespace DB
{
    using FactoryFunction = std::function< std::any( const std::string& ) >;
    using SinkFunction = std::function< void( const std::any& ) >;

    struct System
    {
        std::map< std::string, FactoryFunction > factories;
        void Query( std::string what, const SinkFunction& sink )
        {
            auto dataLines = std::vector< std::pair< std::string, std::string > >  
            {   { "i", "1" }
            ,   { "i", "4711" }
            ,   { "s", "hello" }
            ,   { "s", "world" }
            ,   { "d", "3.14" }
            };
            for( auto [ type, data ] : dataLines )
                sink( factories[ type ]( data ) );
        }        
    };
}

namespace Application
{
    struct IntData
    {
        int data = 0;
    }; 
    struct StringData
    {
        std::string data;
    }; 
    struct DoubleData
    {
        double data;
    }; 

    auto AnyToOut = library::declare_any_dispatch< void, const std::any& >{};

    void IntToOut( const IntData* i ){ std::cout << "int: " << i->data << std::endl; }

    void AnywhereInTheApplication()
    {
        AnyToOut.define< const IntData >( &IntToOut );
    }
}

int main()
{
    using namespace Application;

    AnywhereInTheApplication();

    DB::System db;

    AnyToOut.define< const StringData >( []( const auto s )
        { std::cout << "string: " << s->data << std::endl; } );

    db.factories[ "i" ] = []( const std::string& data )->std::any{  return IntData{std::atoi( data.c_str() ) }; };
    db.factories[ "s" ] = []( const std::string& data )->std::any{  return StringData{ data }; };
    db.factories[ "d" ] = []( const std::string& data )->std::any{  return DoubleData{ std::atof( data.c_str() ) }; };
    try
    {
        db.Query( "junk", AnyToOut );
    }
    catch( std::exception& e )
    {
        std::cout << "error: " << e.what() << std::endl;
    }
    return 0;
}