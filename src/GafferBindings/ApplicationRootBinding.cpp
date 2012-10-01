//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2011, John Haddon. All rights reserved.
//  Copyright (c) 2011-2012, Image Engine Design Inc. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#include "boost/python.hpp"

#include <fstream>

#include "boost/filesystem.hpp"

#include "IECorePython/ScopedGILLock.h"
#include "IECorePython/RunTimeTypedBinding.h"
#include "IECorePython/Wrapper.h"

#include "Gaffer/ApplicationRoot.h"
#include "Gaffer/CompoundPlug.h"
#include "Gaffer/PreferencesNode.h"

#include "GafferBindings/ApplicationRootBinding.h"
#include "GafferBindings/Serialiser.h"
#include "GafferBindings/ValuePlugBinding.h"

using namespace boost::python;
using namespace GafferBindings;
using namespace Gaffer;

class ApplicationRootWrapper : public ApplicationRoot, public IECorePython::Wrapper<ApplicationRoot>
{

	public :

		ApplicationRootWrapper( PyObject *self, const std::string &name = staticTypeName() )
			:	ApplicationRoot( name ), IECorePython::Wrapper<ApplicationRoot>( self, this )
		{
		}
	
		virtual void savePreferences( const std::string &fileName ) const
		{
			IECorePython::ScopedGILLock gilLock;
			
			// serialise everything
			Serialiser s( preferences() );
			serialisePlugs( s, preferences(), "application.root()[\"preferences\"]" );
			
			// make the directory for the preferences file if it doesn't exist yet
			boost::filesystem::path path( fileName );
			path.remove_filename();
			boost::filesystem::create_directories( path );
		
			// then make the file and write the serialisation into it
			std::ofstream f;
			f.open( fileName.c_str() );
			if( !f.is_open() )
			{
				throw IECore::Exception( "Unable to open file \"" + fileName + "\"" );
			}
						
			f << "# This file was automatically generated by Gaffer.\n";
			f << "# Do not edit this file - it will be overwritten.\n\n";
			f << s.result() << std::endl;	
			
			if( !f.good() )
			{
				throw IECore::Exception( "Error while writing file \"" + fileName + "\"" );			
			}
		}
		
		void serialisePlugs( Serialiser &s, ConstGraphComponentPtr parent, const std::string &parentPath ) const
		{
			InputPlugIterator it( parent->children().begin(), parent->children().end() );
			InputPlugIterator end( parent->children().end(), parent->children().end() );

			for( ; it!=end; it++ )
			{
				if( (*it)->typeId() == CompoundPlug::staticTypeId() )
				{
					serialisePlugs( s, *it, parentPath + "[\"" + (*it)->getName() + "\"]" );
				}
				else
				{
					std::string ss = parentPath + "[\"" + (*it)->getName() + "\"].setValue( ";
					ss += serialisePlugValue( s, *it );
					ss += " )\n";
					
					s.add( ss );
				}
			}
			
		
		}

};

IE_CORE_DECLAREPTR( ApplicationRootWrapper )

static IECore::ObjectPtr getClipboardContents( ApplicationRoot &a )
{
	IECore::ConstObjectPtr o = a.getClipboardContents();
	if( o )
	{
		return o->copy();
	}
	return 0;
}

void GafferBindings::bindApplicationRoot()
{
	IECorePython::RunTimeTypedClass<ApplicationRoot, ApplicationRootWrapperPtr>()
		.def( init<>() )
		.def( init<const std::string &>() )
		.def( "getClipboardContents", &getClipboardContents )
		.def( "setClipboardContents", &ApplicationRoot::setClipboardContents )
		.def( "savePreferences", (void (ApplicationRoot::*)()const)&ApplicationRoot::savePreferences )
		.def( "savePreferences", (void (ApplicationRoot::*)( const std::string & )const)&ApplicationRoot::savePreferences )
		.def( "preferencesLocation", &ApplicationRoot::preferencesLocation )
	;	
}
