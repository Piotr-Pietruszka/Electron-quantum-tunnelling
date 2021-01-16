function wavemovie

v = VideoWriter('schrodinger.avi', 'Uncompressed AVI');
v.FrameRate = 10;
open(v)


% First file to find limits
fptr = fopen("real_part" + 50 + ".bin");
[array, nx] = fread(fptr,'float');
fclose(fptr);
my_ylimits = [min(array), max(array)];

fptr = fopen("imag_part" + 50 + ".bin");
[array, nx] = fread(fptr,'float');
fclose(fptr);
my_ylimits2 = [min(array), max(array)];
if my_ylimits2(1) < my_ylimits(1)
     my_ylimits(1) =  my_ylimits2(1);
end
if my_ylimits2(2) > my_ylimits(2)
     my_ylimits(2) =  my_ylimits2(2);
end
my_ylimits(1) = my_ylimits(1) - 0.1*(max(array) - min(array));
my_ylimits(2) = my_ylimits(2) + 0.1*(max(array) - min(array));

% Read kinetic and potetnial energy and potential barrier size
fptr=fopen("kinetic_energy.bin");
[kinetic_energy]=fread(fptr,'float');
fptr=fopen("potential_energy.bin");
[potential_energy]=fread(fptr,'float');
fclose(fptr);
fptr=fopen("vp.bin");
[potential, nx]=fread(fptr,'float');
fclose(fptr);

for t=50:50:7950;
    filename = "real_part" + t + ".bin";
    real_part = plotwaveform(filename, my_ylimits);
    hold on
    
    filename = "imag_part" + t + ".bin";
    imag_part = plotwaveform(filename, my_ylimits);
    
    plot(sqrt(real_part.*real_part + imag_part.*imag_part));
    
    plotpotential("vp.bin", my_ylimits);
    text(nx/6, my_ylimits(2)*4/5, "Kinetic energy: " + kinetic_energy(t/50)*6.28e18 + "eV")
    text(nx*4/6, my_ylimits(2)*4/5, "Potential energy: " + potential_energy(t/50)*6.28e18 + "eV")
    text(nx/6, my_ylimits(1)*4/5, "Total energy: " + (potential_energy(t/50)*6.28e18 + kinetic_energy(t/50)*6.28e18) + "eV")
    text(nx*4/6, my_ylimits(1)*4/5, "Potential barrier: " + (max(potential)*6.28e18) + "eV")
    
    filename = "imag_part" + t + ".bin";
    plotwaveform(filename, my_ylimits);
    
    legend("Real part", "Imaginary part", "Amplitude", "Potential", 'Location', 'NorthOutside');
    hold off
    
    frame = getframe(gcf);
    writeVideo(v, frame);
    %disp(filename);
end

close(v);