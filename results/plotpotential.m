function plotpotential(filename, my_ylim)

	fptr=fopen(filename);

	[array, nx]=fread(fptr,'float');
    max_potential = max(array);
    min_potential = min(array);

	if nargin == 2
		ylim(my_ylim);
        
        if min_potential / my_ylim(1) > max_potential  / my_ylim(2) 
           array = array ./ min_potential  .* 0.5 .* my_ylim(1);
        else
           array = array ./ max_potential  .* 0.5 .* my_ylim(2);
        end  
    end
    plot(array, '-');
	fclose(fptr);