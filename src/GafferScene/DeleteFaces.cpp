//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2017, Image Engine Design Inc. All rights reserved.
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
//      * Neither the name of Image Engine Design Inc nor the names of
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

#include "boost/format.hpp"
#include "boost/algorithm/string.hpp"

#include "IECoreScene/MeshPrimitive.h"
#include "IECoreScene/MeshAlgo.h"

#include "Gaffer/StringPlug.h"

#include "GafferScene/DeleteFaces.h"

using namespace IECore;
using namespace IECoreScene;
using namespace Gaffer;
using namespace GafferScene;

IE_CORE_DEFINERUNTIMETYPED( DeleteFaces );

size_t DeleteFaces::g_firstPlugIndex = 0;

DeleteFaces::DeleteFaces( const std::string &name ) : SceneElementProcessor( name, IECore::PathMatcher::NoMatch )
{
	storeIndexOfNextChild( g_firstPlugIndex );

	addChild( new StringPlug( "faces", Plug::In, "deleteFaces" ) );

	// Fast pass-through for things we don't modify
	outPlug()->attributesPlug()->setInput( inPlug()->attributesPlug() );
	outPlug()->transformPlug()->setInput( inPlug()->transformPlug() );
}

DeleteFaces::~DeleteFaces()
{
}

Gaffer::StringPlug *DeleteFaces::facesPlug()
{
	return getChild<StringPlug>( g_firstPlugIndex );
}

const Gaffer::StringPlug *DeleteFaces::facesPlug() const
{
	return getChild<StringPlug>( g_firstPlugIndex );
}

void DeleteFaces::affects( const Gaffer::Plug *input, AffectedPlugsContainer &outputs ) const
{
	SceneElementProcessor::affects( input, outputs );

	if( input == facesPlug() )
	{
		outputs.push_back( outPlug()->objectPlug() );
	}
	else if( input == outPlug()->objectPlug() )
	{
		outputs.push_back( outPlug()->boundPlug() );
	}
}

bool DeleteFaces::processesBound() const
{
	return true;
}

void DeleteFaces::hashProcessedBound( const ScenePath &path, const Gaffer::Context *context, IECore::MurmurHash &h ) const
{

	hashProcessedObject( path, context, h );
}

Imath::Box3f DeleteFaces::computeProcessedBound( const ScenePath &path, const Gaffer::Context *context, const Imath::Box3f &inputBound ) const
{
	ConstObjectPtr object = outPlug()->objectPlug()->getValue();
	if( const Primitive *primitive = runTimeCast<const Primitive>( object.get() ) )
	{
		return primitive->bound();
	}
	return inputBound;
}

bool DeleteFaces::processesObject() const
{
	return true;
}

void DeleteFaces::hashProcessedObject( const ScenePath &path, const Gaffer::Context *context, IECore::MurmurHash &h ) const
{
	facesPlug()->hash( h );
}

IECore::ConstObjectPtr DeleteFaces::computeProcessedObject( const ScenePath &path, const Gaffer::Context *context, IECore::ConstObjectPtr inputObject ) const
{
	const MeshPrimitive *mesh = runTimeCast<const MeshPrimitive>( inputObject.get() );
	if( !mesh )
	{
		return inputObject;
	}


	std::string deletePrimVarName = facesPlug()->getValue();

	if( boost::trim_copy( deletePrimVarName ).empty() )
	{
		return inputObject;
	}

	PrimitiveVariableMap::const_iterator it = mesh->variables.find( deletePrimVarName );
	if( it == mesh->variables.end() )
	{
		throw InvalidArgumentException( boost::str( boost::format( "DeleteFaces : No primitive variable \"%s\" found" ) % deletePrimVarName ) );
	}

	return MeshAlgo::deleteFaces( mesh, it->second );
}
