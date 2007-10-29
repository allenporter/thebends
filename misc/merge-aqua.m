I = imread('aqua-hidden.jpg');
J = imread('aqua-visible.jpg');
K = merge(I, J);
imwrite(K, 'aqua-mixed.jpg');