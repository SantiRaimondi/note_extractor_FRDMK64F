% Nayiri Krzysztofowicz
% Spring 2019
% main program to extract notes from audio file
% input file: FurElise_Slow.mp3
% relies on plotsound.m file for one (optional) function

%% set up song
clear; clc; clf; close all

mute = false; % set this to false to hear audio throughout program
             % useful for debugging

[song,Fs] = audioread('Fur elise/FurElise_Slow.mp3');
Fs = Fs*4;   % speed up song (original audio file is very slow) NO HACE FALTA PARA OTRAS CANCIONES
figure, plot(song(:,1)), title('Fur Elise, entire song')
%% set parameters (change based on song)
t1 = 2.9e6; t2 = 4.9e6; % Fur Elise

% analyze a window of the song
y = song(t1:t2);
[~,n] = size(y);
t = linspace(t1,t2,n);
% if ~mute, plotsound(y,Fs); end
audiowrite('Fur elise/fur_elise_window.wav',y,Fs);
%% FFT of song
% Y = fft(y);
% Y_norm = abs(Y./max(Y));
% figure, plot(Y_norm), title('Normalized FFT of song window'), xlim([0 floor(n/2)])

%% downsample by m
clc, close all
m = 20;   
% m = 10 % Harry Potter
Fsm = round(Fs/m);
p = floor(n/m);
y_avg = zeros(1,p);
for i = 1:p
    y_avg(i) = mean(y(m*(i-1)+1:m*i));
end

figure, plot(linspace(0,100,n),abs(y)), hold on
        plot(linspace(0,100,p),abs(y_avg))
        title('Discrete notes of song')
        legend('Original', '20-point averaged and down-sampled')
if ~mute, sound(y_avg,Fsm); end
audiowrite('Fur elise/FurElise_downsampled.wav',y_avg,Fsm);

%% threshold to find notes
% Lo que hace es detectar una tendencia ascendente en el valor de y_avg_uint8

close all

y_thresh = zeros(1,p); 
i = 1;
while (i <= p)
    thresh = 5*median(abs(y_avg(max(1,i-5000):i)));    
    if (abs(y_avg(i)) > thresh)                        
        for j = 0:500                                  
            if (i + j <= p)
                y_thresh(i) = y_avg(i);
                i = i + 1;
            end
        end
        i = i + 1400;   % Pega el salto de 1400 para garantizar saltear las muestras que ya guarde,
                        % para que no entre nuevamente al if() con valores que son menores a los guardados anteriormente
    end
    i = i + 1;
end

figure, subplot(2,1,1), plot(abs(y_avg)), title('Original song'), ylim([0 1.1*max(y_avg)])
        subplot(2,1,2), plot(abs(y_thresh)), title('Detected notes using moving threshold')

% Se guarda en un archivo .h para poder despues cargarlo a la placa y hacer
% la FFT sobre esos valores.
fileID = fopen('Fur elise/y_thresh_avg.h','w');
fprintf(fileID, '#define DATA_SIZE %d \r\n\n', p);
fprintf(fileID, 'const float32_t data_thresh[DATA_SIZE] = { \r\n');
fprintf(fileID,'%d,\t %d,\t %d,\t %d,\t %d,\t %d,\t %d,\t %d,\t %d,\t %d, \r\n', single(y_thresh));
fprintf(fileID, '}; \r\n');
fclose(fileID);
        
if ~mute, sound(y_thresh,round(Fsm)); end


%% find frequencies of each note
% Este bloque se encarga de recorrer el arreglo con los sonidos que
% superaron el umbral, para calcular la FFT solo sobre los sonidos dejando
% fuera del cálculo a los momentos de silencio.
% Funciona detectando una cantidad de ceros consecutivos que representan el
% inicio del silencio. En este caso son 20 ceros consecutivos (end_note).
% Luego de calcular la FFT se lee el indice del valor absoluto máximo. No
% entiendo porque para detectar la frecuencia que representa ese indice lo
% multiplica por 2.
    clc; close all

    i = 1;
    i_note = 0;
    while i < p
        j = 1;
        end_note = 0;
        while (((y_thresh(i) ~= 0) || (end_note > 0)) && (i < p))
            note(j) = y_thresh(i);
            i = i + 1;
            j = j + 1;
            if (y_thresh(i) ~= 0)   % Si detecta 20 0's seguidos quiere decir que estoy leyendo la 
                end_note = 20;      % parte de los silencios (no hay una tecla que haya sido apretada)
            else
                end_note = end_note - 1;
            end

            if (end_note == 0)  % Si ya guarde en note() todos los valores del sonido(500 valores) mas 20 0's (detecto end_note = 0), entro al if()
               if (j > 25)      % Si almacene al menos 25 muestras prosigo
                   note_padded = [note zeros(1,j)]; % pad note with zeros to double size (N --> 2*N-1)
                   Note = fft(note_padded);
                   Ns = length(note); % Cantidad de items (muestras de y_thresh()) en note(). En todos las iteraciones Ns = 520 (le saque el ';' para que imprima por command window).
                                      % Segun mis calculos deberia ser igual a j (de hecho creo que es asi segun la linea: note_padded = [note zeros(1,j)];). 'j' es 521 xq empieza en 1.
                   f = linspace(0,(1+Ns/2),Ns); % Esto crea un arreglo de Ns valores equiespaciados. El espaciado es ((1+Ns/2)-0)/(Ns-1)
                   [val,index] = max(abs(Note(1:length(f)))); % Saca el indice del lobulo de mayor energia
                   if (f(index) > 20)   % Descarta los valores de frecuencia menores a 20 Hz.
                       i_note = i_note + 1;        % Contador de notas encontradas.
                       fundamentals(i_note) = f(index)*2    % Multiplica x2 porque la FFT solo analiza la mitad de rango de la frec. de muestreo
                       figure, plot(f,abs(Note(1:length(f))))
                               title(['Fundamental frequency = ',num2str(fundamentals(i_note)),' Hz'])
                               %plot(note_padded)
                   end
                   i = i + 50; % Pego un acelerón en el índice i para ahorrar repeticiones leyendo silencios.
               end
               clear note;
               break
            end

        end
        i = i + 1;
    end
    

%% play back notes
clc; close all
amp = 0.05; % Se la reduje para que no me mate los timpanos
% fs = 20500;  % sampling frequency
fs = 3000;
duration = .5;  % Duracion que el pianista va a estar tocando cada nota
recreate_song = zeros(1,duration*fs*length(fundamentals));
for i = 1:length(fundamentals)
    [letter(i,1),freq(i)]= FreqToNote(fundamentals(i)); % Con esta funcion se aproxima la frecuencia fundamental de la nota tocada a la nota de la escala exacta mas proxima
    values = 0:1/fs:duration;           % Vector de 'duration' cantidad de pts, separados 1/fs
    a = amp*sin(2*pi*freq(i)*values*2); % Se crea un arreglo (a) con el tono de la cancion. No se porque x2.
    recreate_song((i-1)*fs*duration+1:i*fs*duration+1) = a;
    
    % Para ver la forma de onda del tono que se reproduce:
%     figure, plot(values,a)
%     ylim([-1.1*amp 1.1*amp])
%     title(['Tone frequency = ',num2str(fundamentals(i)),' Hz', 'Note: ', letter(i), 'Note frec.:', num2str(freq(i))])

    if ~mute, sound(a,fs); pause(.5); end
end

audiowrite('Fur elise/fur_elise_recreated.wav',recreate_song,fs);

