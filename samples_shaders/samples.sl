surface my_constant()
{
	Oi = Os;
	Ci = Os * Cs;
}

surface surfaceNormals()
{
	vector Nf = faceforward(N, I);
	Oi = Os;
	Ci = Os * (Nf + 1) * 0.5;
}

surface viewI()
{
	Oi = Os;
	Ci = Os * (I + 1) * 0.5;
}

surface diffuse_sample()
{
	Oi = Os;
	Ci = Os * Cs * diffuse(N);
}

surface specular_sample(float roughness=0.1;)
{
	Oi = Os;
	Ci = Os * Cs * (diffuse(N) * 0.5 + 0.5 * specular(N, -I, roughness));
}

surface reflect_sample(
						float Ka = 1;
						float Kd = 0.5;
						float Ks = 0.5;
						float roughness = 0.1;
						float reflectStrengh = 1;)
{
	vector Nf = faceforward(N, I);
	vector V = -normalize(I);
	vector r = reflect(I, Nf);
	color lighting = Ka * ambient() + Kd * diffuse(Nf) + Ks * specular(Nf, V, roughness);
	Oi = Os;
	Ci = Os * Cs * ((1 - reflectStrengh) * lighting + trace(P, r) * reflectStrengh);
	//Ci = (r+1)*0.5;
}

surface reflect_sample_additive(
						float Ka = 1;
						float Kd = 0.5;
						float Ks = 0.5;
						float roughness = 0.1;
						float reflectStrengh = 1;)
{
	vector Nf = faceforward(N, I);
	vector V = -normalize(I);
	vector r = reflect(I, Nf);
	color lighting = Ka * ambient() + Kd * diffuse(Nf) + Ks * specular(Nf, V, roughness);
	Oi = Os;
	Ci = Os * Cs * lighting + trace(P, r) * reflectStrengh;
	//Ci = (r+1)*0.5;
}

surface matte_opa(float Ka = 1;
				float Kd = 1;)
{
	vector Nf = faceforward(N, I);
	Oi = Os;
	Ci = Os * Cs * (Kd * diffuse(N));
}

surface smoothstep_sample(float rim_width = 0.5;)
{
	vector nf = faceforward(N, I);
	vector i = normalize(-I);
	float dot = 1 - nf.i;
	float diffusecolor = smoothstep(1 - rim_width, 1, dot);
	
	Oi = Os;
	Ci = Oi * Cs * diffusecolor;
}

surface mix_sample()
{
	color top = color(0.878, 0.996, 0.250);
	color low = color(0.250, 0.878, 0.988);
	Oi = Os;
	Ci = Oi * Cs * (diffuse(N) + ambient()) * mix(top, low, t);
}
