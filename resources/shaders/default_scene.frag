// osgOcean Uniforms
// -----------------
uniform float osgOcean_DOF_Near;
uniform float osgOcean_DOF_Focus;
uniform float osgOcean_DOF_Far;
uniform float osgOcean_DOF_Clamp;

uniform float osgOcean_UnderwaterFogDensity;
uniform float osgOcean_AboveWaterFogDensity;
uniform vec4  osgOcean_UnderwaterFogColor;
uniform vec4  osgOcean_AboveWaterFogColor;

uniform float osgOcean_WaterHeight;

uniform bool osgOcean_EnableGlare;
uniform bool osgOcean_EnableDOF;
uniform bool osgOcean_EyeUnderwater;
// -------------------

uniform sampler2D uTextureMap;
uniform sampler2D SLStex;
uniform sampler2D SLStex2;

varying vec3 vExtinction;
varying vec3 vInScattering;
varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vEyeVec;
varying vec4 color;

varying vec4 ShadowCoord;
uniform bool isLaser;


varying float vWorldHeight;

float computeDepthBlur(float depth, float focus, float near, float far, float clampval )
{
	float f;
	if (depth < focus){
		f = (depth - focus)/(focus - near);
	}
	else{
		f = (depth - focus)/(far - focus);
		f = clamp(f, 0.0, clampval);
	}
	return f * 0.5 + 0.5;
}

vec4 lighting( vec4 colormap )
{
	vec4 final_color = gl_LightSource[osgOcean_LightID].ambient * colormap;

	vec3 N = normalize(vNormal);
	vec3 L = normalize(vLightDir);

	float lambertTerm = dot(N,L);

	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[osgOcean_LightID].diffuse * lambertTerm * colormap;

		vec3 E = normalize(vEyeVec);
		vec3 R = reflect(-L, N);

		float specular = pow( max(dot(R, E), 0.0), 2.0 );

		final_color += gl_LightSource[osgOcean_LightID].specular * specular;
	}

	return final_color;
}

float computeFogFactor( float density, float fogCoord )
{
	return exp2(density * fogCoord * fogCoord );
}

void main()
{
	vec4 textureColor;

	// CHECK Shadowed elements in laser (0.5 shadow, 1.0 clear)
	vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
	
	// Used to lower moiré pattern and self-shadowing
	shadowCoordinateWdivide.z -= 0.005;

	float distanceFromLight = texture2D(SLStex,shadowCoordinateWdivide.st).z;
	
	float shadow = 1.0;
	if (ShadowCoord.w > 0.0 )
	{
		shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
	}
	// END CHECK Shadowed elements in laser (0.5 shadow, 1.0 clear)

	// Set objects texture color if they have
	if(texture2D( uTextureMap, gl_TexCoord[0].st )!=vec4(1,1,1,1))
	{
		textureColor = texture2D( uTextureMap, gl_TexCoord[0].st );
	}
	else
	{
		textureColor = color;
	}

	// set default light color
	vec4 lightColor = vec4(0.0,0.0,0.0,0.0);

	// get SLS texture
	vec4 texcolor=texture2D( SLStex2, shadowCoordinateWdivide.st );

	//check SLS texture for backprojection, shadow and out of texture bounds
	if(distanceFromLight>0.0 && ShadowCoord.w > 0.20 && shadow!=0.5 && texcolor!=vec4(1.0,1.0,1.0,1.0) && texcolor.w>0)
	{
		if(isLaser)//treating as laser projection (not dependent on the distance, substitutes original color)
		{ 
			if (round(texcolor.x)+round(texcolor.y)+round(texcolor.z)>0)
			{
				textureColor = vec4(round(texcolor.x),round(texcolor.y),round(texcolor.z),1.0);
			}
		}
		else //treating as light projection (dependent on the distance, added to original color)
		{
			lightColor.w = 1;
			lightColor.xyz = texcolor.xyz/(distanceFromLight*distanceFromLight);
		}
	}

	vec4 final_color;
	float alpha;

	// Underwater
	// +2 tweak here as waves peak above average wave height,
	// and surface fog becomes visible.
	if(osgOcean_EyeUnderwater && vWorldHeight < osgOcean_WaterHeight+2.0 )
	{
		final_color = lighting( textureColor)+lightColor;
		
		// mix in underwater light
		final_color.rgb = final_color.rgb * vExtinction + vInScattering;

		float fogFactor = computeFogFactor( osgOcean_UnderwaterFogDensity, gl_FogFragCoord );

		final_color = mix( osgOcean_UnderwaterFogColor, final_color, fogFactor );

		if(osgOcean_EnableDOF)
		{
			final_color.a = computeDepthBlur(gl_FogFragCoord, osgOcean_DOF_Focus, osgOcean_DOF_Near, osgOcean_DOF_Far, osgOcean_DOF_Clamp);
		}
	}
	// Above water
	else
	{
		final_color = lighting( textureColor)+lightColor;

		float fogFactor = computeFogFactor( osgOcean_AboveWaterFogDensity, gl_FogFragCoord );
		final_color = mix( osgOcean_AboveWaterFogColor, final_color, fogFactor );

		if(osgOcean_EnableGlare)
		{
			final_color.a = 0.0;
		}
	}

	gl_FragColor = final_color;
}
