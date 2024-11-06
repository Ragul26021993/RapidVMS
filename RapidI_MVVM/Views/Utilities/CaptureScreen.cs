using System;
using System.IO;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Linq;
using System.Linq.Expressions;
using System.ComponentModel;
using System.Collections.Generic;


namespace Rapid.Utilities
{
    public static class Screenshot
    {
        public static byte[] GetJpgImage(this UIElement source, double scale, int quality)
        {
            double actualHeight = source.RenderSize.Height;
            double actualWidth = source.RenderSize.Width;

            double renderHeight = actualHeight * scale;
            double renderWidth = actualWidth * scale;

            RenderTargetBitmap renderTarget = new RenderTargetBitmap((int)renderWidth, (int)renderHeight, 96, 96, PixelFormats.Pbgra32);
            VisualBrush sourceBrush = new VisualBrush(source);

            DrawingVisual drawingVisual = new DrawingVisual();
            DrawingContext drawingContext = drawingVisual.RenderOpen();

            using (drawingContext)
            {
                drawingContext.PushTransform(new ScaleTransform(scale, scale));
                drawingContext.DrawRectangle(sourceBrush, null, new Rect(new System.Windows.Point(0, 0), new System.Windows.Point(actualWidth, actualHeight)));
            }
            renderTarget.Render(drawingVisual);

            JpegBitmapEncoder jpgEncoder = new JpegBitmapEncoder();
            jpgEncoder.QualityLevel = quality;
            jpgEncoder.Frames.Add(BitmapFrame.Create(renderTarget));

            Byte[] _imageArray;

            using (MemoryStream outputStream = new MemoryStream())
            {
                jpgEncoder.Save(outputStream);
                _imageArray = outputStream.ToArray();
            }

            return _imageArray;
        }
        public static void SaveScreen(this object Source, string Imagepath = "screen.Bmp")
        {
            //Rectangle region = new Rectangle(SystemInformation.VirtualScreen.Left, SystemInformation.VirtualScreen.Top, SystemInformation.VirtualScreen.Width, SystemInformation.VirtualScreen.Height);
            Rectangle region = new Rectangle(Screen.PrimaryScreen.Bounds.X, Screen.PrimaryScreen.Bounds.Y, Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Height);
            using (Bitmap bitmap = new Bitmap(region.Width, region.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb))
            using (Graphics bitmapGraphics = Graphics.FromImage(bitmap))
            {
                bitmapGraphics.CopyFromScreen(region.Left, region.Top, 0, 0, region.Size);
                //bitmap.Save("screen.png", ImageFormat.Png);
                bitmap.Save(Imagepath, ImageFormat.Bmp);
            }
        }
        public static Image Crop(this Image image, Rectangle selection)
        {
            try
            {
                Bitmap bmp = image as Bitmap;
                Bitmap cropBmp = null;
                // Check if it is a bitmap:
                if (bmp == null)
                    throw new ArgumentException("No valid bitmap");
                if (selection != null)
                {
                    // Crop the image:
                    cropBmp = bmp.Clone(selection, bmp.PixelFormat);

                    // Release the resources:
                    image.Dispose();
                }

                return cropBmp;
            }
            catch (Exception)
            {
                return null;
            }
        }
        public static Image Fit2PictureBox(this Image image, PictureBox picBox)
        {
            Bitmap bmp = null;
            Graphics g;

            // Scale:
            double scaleY = (double)image.Width / picBox.Width;
            double scaleX = (double)image.Height / picBox.Height;
            double scale = scaleY < scaleX ? scaleX : scaleY;

            // Create new bitmap:
            bmp = new Bitmap(
                (int)((double)image.Width / scale),
                (int)((double)image.Height / scale));

            // Set resolution of the new image:
            bmp.SetResolution(
                image.HorizontalResolution,
                image.VerticalResolution);

            // Create graphics:
            g = Graphics.FromImage(bmp);

            // Set interpolation mode:
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;

            // Draw the new image:
            g.DrawImage(
                image,
                new Rectangle(            // Destination
                    0, 0,
                    bmp.Width, bmp.Height),
                new Rectangle(            // Source
                    0, 0,
                    image.Width, image.Height),
                GraphicsUnit.Pixel);

            // Release the resources of the graphics:
            g.Dispose();

            // Release the resources of the origin image:
            image.Dispose();

            return bmp;
        }
        public static Image ScaleImage(this Image image, int maxWidth, int maxHeight, int quality)
        {
            // Get the image's original width and height
            int originalWidth = image.Width;
            int originalHeight = image.Height;

            // To preserve the aspect ratio
            float ratioX = (float)maxWidth / (float)originalWidth;
            float ratioY = (float)maxHeight / (float)originalHeight;
            float ratio = Math.Min(ratioX, ratioY);

            // New width and height based on aspect ratio
            int newWidth = (int)(originalWidth * ratio);
            int newHeight = (int)(originalHeight * ratio);

            // Convert other formats (including CMYK) to RGB.
            Bitmap newImage = new Bitmap(newWidth, newHeight, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

            // Draws the image in the specified size with quality mode set to HighQuality
            using (Graphics graphics = Graphics.FromImage(newImage))
            {
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.DrawImage(image, 0, 0, newWidth, newHeight);
            }

            // Get an ImageCodecInfo object that represents the JPEG codec.
            ImageCodecInfo imageCodecInfo = GetEncoderInfo(ImageFormat.Jpeg);

            // Create an Encoder object for the Quality parameter.
            Encoder encoder = Encoder.Quality;

            // Create an EncoderParameters object. 
            EncoderParameters encoderParameters = new EncoderParameters(1);

            // Save the image as a JPEG file with quality level.
            EncoderParameter encoderParameter = new EncoderParameter(encoder, quality);
            encoderParameters.Param[0] = encoderParameter;
            //newImage.Save(filePath, imageCodecInfo, encoderParameters);
            return newImage;
        }
        public static Image ScaleImage(this Image image, float MagRatio, int quality)
        {
            try
            {
                // Get the image's original width and height
                int originalWidth = image.Width;
                int originalHeight = image.Height;

                // To preserve the aspect ratio           
                float ratio = MagRatio;

                // New width and height based on aspect ratio
                int newWidth = (int)(originalWidth * ratio);
                int newHeight = (int)(originalHeight * ratio);

                // Convert other formats (including CMYK) to RGB.
                Bitmap newImage = new Bitmap(newWidth, newHeight, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

                // Draws the image in the specified size with quality mode set to HighQuality
                using (Graphics graphics = Graphics.FromImage(newImage))
                {
                    graphics.CompositingQuality = CompositingQuality.HighQuality;
                    graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                    graphics.SmoothingMode = SmoothingMode.HighQuality;
                    graphics.DrawImage(image, 0, 0, newWidth, newHeight);
                }

                // Get an ImageCodecInfo object that represents the JPEG codec.
                ImageCodecInfo imageCodecInfo = GetEncoderInfo(ImageFormat.Jpeg);

                // Create an Encoder object for the Quality parameter.
                Encoder encoder = Encoder.Quality;

                // Create an EncoderParameters object. 
                EncoderParameters encoderParameters = new EncoderParameters(1);

                // Save the image as a JPEG file with quality level.
                EncoderParameter encoderParameter = new EncoderParameter(encoder, quality);
                encoderParameters.Param[0] = encoderParameter;
                //newImage.Save(filePath, imageCodecInfo, encoderParameters);
                return newImage;
            }
            catch (Exception)
            {
                throw;
            }
        }
        public static byte[] ImageToByte(this object Source, Image img)
        {
            ImageConverter converter = new ImageConverter();
            return (byte[])converter.ConvertTo(img, typeof(byte[]));
        }

        public static bool ChangeAndNotify<T>(this PropertyChangedEventHandler handler, ref T field, T value, Expression<Func<T>> memberExpression)
        {
            if (memberExpression == null)
            {
                throw new ArgumentNullException("memberExpression");
            }
            var body = memberExpression.Body as MemberExpression;
            if (body == null)
            {
                throw new ArgumentException("Lambda must return a property.");
            }
            if (EqualityComparer<T>.Default.Equals(field, value))
            {
                return false;
            }

            var vmExpression = body.Expression as ConstantExpression;
            if (vmExpression != null)
            {
                LambdaExpression lambda = System.Linq.Expressions.LambdaExpression.Lambda(vmExpression);
                Delegate vmFunc = lambda.Compile();
                object sender = vmFunc.DynamicInvoke();

                if (handler != null)
                {
                    handler(sender, new PropertyChangedEventArgs(body.Member.Name));
                }
            }

            field = value;
            return true;
        }

        /// <summary>
        /// Method to get encoder infor for given image format.
        /// </summary>
        /// <param name="format">Image format</param>
        /// <returns>image codec info.</returns>
        private static ImageCodecInfo GetEncoderInfo(ImageFormat format)
        {
            return ImageCodecInfo.GetImageDecoders().SingleOrDefault(c => c.FormatID == format.Guid);
        }
    }

    //private class StateObjClass
    //{
    //    // Used to hold parameters for calls to TimerTask. 
    //    public int SomeValue;
    //    public System.Threading.Timer TimerReference;
    //    public bool TimerCanceled;
    //}
}