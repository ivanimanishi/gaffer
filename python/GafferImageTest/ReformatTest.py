##########################################################################
#  
#  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#  
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#  
#      * Neither the name of John Haddon nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#  
##########################################################################

import unittest

import IECore
import Gaffer
import GafferImage
import os

class ReformatTest( unittest.TestCase ) :

	checkerFile = os.path.expandvars( "$GAFFER_ROOT/python/GafferTest/images/checker.exr" )
	
	# Test that when the input and output format are the same that the hash is passed through.
	def testHashPassThrough( self ) :
		read = GafferImage.ImageReader()
		read["fileName"].setValue( self.checkerFile )
		readFormat = read["out"]["format"].getValue()
		
		# Set the reformat node's format plug to be the same as the read node.
		reformat = GafferImage.Reformat()
		reformat["in"].setInput( read["out"] )
		reformat["format"].setValue( readFormat )
		
		# Check that it passes through the hash for the channel data.
		h1 = read["out"].channelData( "G", IECore.V2i( 0 ) ).hash()
		h2 = reformat["out"].channelData( "G", IECore.V2i( 0 ) ).hash()
		self.assertEqual( h1, h2 )
		
		# Now assert that the hash changes when set to something else.
		reformat["format"].setValue( GafferImage.Format( 128, 256, 1. ) )
		
		h2 = reformat["out"].channelData( "G", IECore.V2i( 0 ) ).hash()
		self.assertNotEqual( h1, h2 )

		

