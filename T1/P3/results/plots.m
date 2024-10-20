figure
plot(quantizationTable.Value, quantizationTable.MSE, '.')
xlabel 'Quantization Level'
ylabel MSE
title("MSE for diferent quantization levels")

figure
plot(quantizationTable.Value, quantizationTable.PSNR, '.')
xlabel 'Quantization Level'
ylabel PSNR
title("PSNR for diferent quantization levels")

figure
plot(quantizationTable.Value, quantizationTable.Timems, '.')
xlabel 'Quantization Level'
ylabel 'Execution Time (ms)'
title("Execution time for diferent quantization levels")

figure
plot(gaussianTable.Value, gaussianTable.MSE, '.')
xlabel 'Kernel Size'
ylabel MSE
title("MSE for diferent kernel sizes")

figure
plot(gaussianTable.Value, gaussianTable.PSNR, '.')
xlabel 'Kernel Size'
ylabel PSNR
title("PSNR for diferent kernel sizes")

figure
plot(gaussianTable.Value, gaussianTable.Timems, '.')
xlabel 'Kernel Size'
ylabel 'Execution Time (ms)'
title("Execution time for diferent kernel sizes")
