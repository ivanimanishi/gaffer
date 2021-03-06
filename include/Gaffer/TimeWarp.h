//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2012, John Haddon. All rights reserved.
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

#ifndef GAFFER_TIMEWARP_H
#define GAFFER_TIMEWARP_H

#include "Gaffer/ContextProcessor.h"
#include "Gaffer/NumericPlug.h"

namespace Gaffer
{

template<typename BaseType>
class IECORE_EXPORT TimeWarp : public ContextProcessor<BaseType>
{

	public :

		IECORE_RUNTIMETYPED_DECLARETEMPLATE( TimeWarp<BaseType>, ContextProcessor<BaseType> );
		IE_CORE_DECLARERUNTIMETYPEDDESCRIPTION( TimeWarp<BaseType> );

		TimeWarp( const std::string &name=GraphComponent::defaultName<TimeWarp>() );
		~TimeWarp() override;

		FloatPlug *speedPlug();
		const FloatPlug *speedPlug() const;

		FloatPlug *offsetPlug();
		const FloatPlug *offsetPlug() const;

	protected :

		bool affectsContext( const Plug *input ) const override;
		void processContext( Context::EditableScope &context ) const override;

};

namespace Detail
{

struct IdentityScope;

} // namespace Detail

/// May be specialised to control the behaviour of
/// TimeWarp<BaseType>.
template<typename BaseType>
struct TimeWarpTraits
{

   /// A class which will be instantiated as
   /// `TimeScope timeScope( Context::current() )`
   /// to modify the context when evaluating the time
   typedef Detail::IdentityScope TimeScope;

};

typedef TimeWarp<ComputeNode> TimeWarpComputeNode;
IE_CORE_DECLAREPTR( TimeWarpComputeNode );

} // namespace Gaffer

#endif // GAFFER_TIMEWARP_H
