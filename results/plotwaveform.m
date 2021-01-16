
function array = plotwaveform(filename, my_ylim)

	fptr=fopen(filename);

	[array, nx]=fread(fptr,'float');
	plot(array);
	if nargin == 2
		ylim(my_ylim);
	end

	xlabel('z (cell)');
	ylabel('field value');
    
	fclose(fptr);
    
end
