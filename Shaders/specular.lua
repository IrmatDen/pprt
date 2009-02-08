function specularShading()
	out = Color4(1, 0, 0, 1)
	out = out + diffuse(N) * 0.5
	out = out + specular(N, 0.1) * 0.5
	return out
end
