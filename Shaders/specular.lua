function specularShading()
	return Color4(1, 0, 0, 1) + diffuse(N) * 0.5 + specular(N, 0.1) * 0.5
end
