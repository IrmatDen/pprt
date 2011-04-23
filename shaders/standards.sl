surface constant()
{
	Oi = Os;
	Ci = Os * Cs;
}

surface matte(
				real Ka = 1;
				real Kd = 1;)
{
	vec3 Nf = faceforward(N, I);
	Oi = Os;
	Ci = Os * Cs * (Ka * ambient() + Kd * diffuse(Nf));
}

surface metal(
				real Ka = 1;
				real Ks = 1;
				real roughness = 0.1;)
{
	vec3 Nf = faceforward(normalize(N), I);
	vec3 V = -normalize(I);
	Oi = Os;
	Ci = Os * Cs * (Ka * ambient() + Ks * specular(Nf, V, roughness));
}

surface plastic(
				real Ka = 1;
				real Kd = 0.5;
				real Ks = 0.5;
				real roughness = 0.1;
				color specularcolor = 1;)
{
	vec3 Nf = faceforward(normalize(N), I);
	vec3 V = -normalize(I);
	Oi = Os;
	Ci = Os * Cs * (Ka * ambient() + Kd * diffuse(Nf) +
					specularcolor * Ks * specular(Nf, V, roughness));
}