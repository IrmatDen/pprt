surface constant()
{
	Oi = Os;
	Ci = Os * Cs;
}

surface matte(
				float Ka = 1;
				float Kd = 1;)
{
	vector Nf = faceforward(N, I);
	Oi = Os;
	Ci = Os * Cs * (Ka * ambient() + Kd * diffuse(Nf));
}

surface metal(
				float Ka = 1;
				float Ks = 1;
				float roughness = 0.1;)
{
	vector Nf = faceforward(normalize(N), I);
	vector V = -normalize(I);
	Oi = Os;
	Ci = Os * Cs * (Ka * ambient() + Ks * specular(Nf, V, roughness));
}

surface plastic(
				float Ka = 1;
				float Kd = 0.5;
				float Ks = 0.5;
				float roughness = 0.1;
				color specularcolor = 1;)
{
	vector Nf = faceforward(normalize(N), I);
	vector V = -normalize(I);
	Oi = Os;
	Ci = Os * (Cs * (Ka * ambient() + Kd * diffuse(Nf)) +
					specularcolor * Ks * specular(Nf, V, roughness));
}