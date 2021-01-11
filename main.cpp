#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

using namespace std;
typedef float myfloat_t;

int main()
{
    const myfloat_t m = 9.1e-31; // electron mass
    const myfloat_t h = 1.054e-34; // reduced Planck constant

    const myfloat_t dz = 1.0e-11; // space step size
    const myfloat_t dt = 1.0/8.0 * 2*m/h*dz*dz; // time step size

    int domain_size = 800;
    const myfloat_t sigma =  30;
    const myfloat_t lambda = 30;
    int i0 = 200; // source position
    myfloat_t Vpot = 80.0; // potential barrier value in eV
    int barrier_start = 400; // beginning of potetnial barrier
    int barrier_end = 250; // beginning of potetnial barrier
    int Nsteps = 4000; // simulation time
    int sTime = 50; // simulation timeT

    
    myfloat_t *psi_real = new myfloat_t[domain_size];
    myfloat_t *psi_imag = new myfloat_t[domain_size];
    myfloat_t *vp = new myfloat_t[domain_size]; // potential

    myfloat_t ke_real = 0.0, ke_imag = 0.0; // Real and imaginary kinetic energy
    myfloat_t *kinetic_energy = new myfloat_t[Nsteps/sTime]; // total kinetic energy
    myfloat_t *potential_energy = new myfloat_t[Nsteps/sTime];  // total potential energy
    myfloat_t lapl_real = 0.0, lapl_imag = 0.0; // laplacians of total and imaginary field

    
    string filename;

    // Initilize to 0
    for(int i = 0; i < domain_size; i++)
    {
        psi_real[i] = 0.0;
        psi_imag[i] = 0.0;
        vp[i] = 0.0;
    }

    // Initialization - gaussian pulse
    myfloat_t integral_psi = 0.0;
    for(int i = 1; i < domain_size-1; i++)
    {
        psi_real[i] = exp(-0.5*(i-i0)/sigma*(i-i0)/sigma) * cos(2*M_PI*(i-i0)/lambda);
        psi_imag[i] = exp(-0.5*(i-i0)/sigma*(i-i0)/sigma) * sin(2*M_PI*(i-i0)/lambda);
        integral_psi += psi_real[i]*psi_real[i] + psi_imag[i]*psi_imag[i];
    }
    integral_psi = sqrt(integral_psi);

    // Normalization - total probablity should be 1
    for(int i = 1; i < domain_size-1; i++)
    {
        psi_real[i] /= integral_psi;
        psi_imag[i] /= integral_psi;
    }
    
    // Potential initalization
    for(int i = barrier_start; i < barrier_end; i++)
        vp[i] = Vpot*1.602e-19; // potential barrier in J
    
    // Potential to file
    filename = "results/vp.bin";
    std::ofstream outfile_vp (filename, std::ofstream::binary);
    outfile_vp.write(reinterpret_cast<char*>(&domain_size), sizeof(int));
    outfile_vp.write(reinterpret_cast<char*>(vp), sizeof(float)*domain_size);
    outfile_vp.close();

    // Main simulation loop
    for(int n = 1; n < Nsteps; n++)
    {
        // Real part calculation
        for(int i = 1; i < domain_size-1; i++)  // h*dt/(2*m*dz*dz) = 1.0/8.0 
        {
             psi_real[i] += -1.0/8.0*(psi_imag[i+1] - 2*psi_imag[i] + psi_imag[i-1]) + dt/h*vp[i]*psi_imag[i];
        }

        // Imaginary part calculation
         for(int i = 1; i < domain_size-1; i++)
         {
            psi_imag[i] += 1.0/8.0*(psi_real[i+1] - 2*psi_real[i] + psi_real[i-1]) - dt/h*vp[i]*psi_real[i];
         }
        
        if(n%sTime == 0)
        {
            
            // Kinetic and potential energy
            ke_real = 0.0, ke_imag = 0.0, potential_energy[n/sTime-1] = 0.0;
            for(int i = 1; i < domain_size-1; i++)
            {   
                lapl_real = psi_real[i+1] - 2*psi_real[i] + psi_real[i-1];
                lapl_imag = psi_imag[i+1] - 2*psi_imag[i] + psi_imag[i-1];
                ke_real += psi_real[i]*lapl_real + psi_imag[i]*lapl_imag;
                ke_imag += psi_real[i]*lapl_imag - psi_imag[i]*lapl_real;

                potential_energy[n/sTime-1] += vp[i] * (psi_real[i]*psi_real[i] + psi_imag[i]*psi_imag[i]);
            }
            ke_real *= -h/2/m * h/dz/dz;
            ke_imag *= -h/2/m * h/dz/dz;
            kinetic_energy[n/sTime-1] = sqrt(ke_real*ke_real + ke_imag*ke_imag);

            
            // Save results to binary files
            // Real part at iteration n
            filename = "results/real_part" + to_string(n) + ".bin";
            std::ofstream outfile_real (filename, std::ofstream::binary);
            outfile_real.write(reinterpret_cast<char*>(&domain_size), sizeof(int));
            outfile_real.write(reinterpret_cast<char*>(psi_real), sizeof(float)*domain_size);
            outfile_real.close();

            // Imaginary part at iteration n
            filename = "results/imag_part" + to_string(n) + ".bin";
            std::ofstream outfile_imag (filename, std::ofstream::binary);
            outfile_imag.write(reinterpret_cast<char*>(&domain_size), sizeof(int));
            outfile_imag.write(reinterpret_cast<char*>(psi_imag), sizeof(float)*domain_size);
            outfile_imag.close();

        }  
    }
    
    
    // Kinetic energy in time
    filename = "results/kinetic_energy.bin";
    std::ofstream outfile_real (filename, std::ofstream::binary);
    outfile_real.write(reinterpret_cast<char*>(kinetic_energy), sizeof(float)*(Nsteps/sTime));
    outfile_real.close();

    // Potential energy in time
    filename = "results/potential_energy.bin";
    std::ofstream outfile_imag (filename, std::ofstream::binary);
    outfile_imag.write(reinterpret_cast<char*>(potential_energy), sizeof(float)*Nsteps/sTime);
    outfile_imag.close();
    
    cout << "Potential barrier: " << Vpot << endl;
    return 0;

    delete [] psi_real;
    delete [] psi_imag;
    delete [] vp; 
}







