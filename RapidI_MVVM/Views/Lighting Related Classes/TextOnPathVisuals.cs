using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Media;

namespace Rapid.LightingControl
{
    public class TextOnPathVisuals : TextOnPathBase
    {
        Typeface typeface;
        protected VisualCollection visualChildren;
        protected List<FormattedText> formattedChars = new List<FormattedText>();
        protected double pathLength;
        protected double textLength;
        protected Rect boundingRect = new Rect();

        public TextOnPathVisuals()
        {
            try
            {
                typeface = new Typeface(FontFamily, FontStyle, FontWeight, FontStretch);
                visualChildren = new VisualCollection(this);
            }
            catch(Exception e)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP07", e);
            }

        }

        protected override void OnPathPropertyChanged(DependencyPropertyChangedEventArgs args)
        {
             try
             {  
                TransformVisualChildren();
             }
             catch (Exception e)
             {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP06", e);
            }
        }

        protected override void OnFontPropertyChanged(DependencyPropertyChangedEventArgs args)
        {
            try
            {
                typeface = new Typeface(FontFamily, FontStyle, FontWeight, FontStretch);
                OnTextPropertyChanged(args);
            }
            catch (Exception e)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP04", e);
            }
        }

        protected override void OnForegroundPropertyChanged(DependencyPropertyChangedEventArgs args)
        {
            try
            {
                OnTextPropertyChanged(args);
            }
            catch(Exception e)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP05", e);
            }
        }

        protected override void OnTextPropertyChanged(DependencyPropertyChangedEventArgs args)
        {
            try
            {
                formattedChars.Clear();
                textLength = 0;

                foreach (char ch in Text)
                {
                    FormattedText formattedText =
                        new FormattedText(ch.ToString(), CultureInfo.CurrentCulture,
                                FlowDirection.LeftToRight, typeface, 12, Foreground);

                    formattedChars.Add(formattedText);
                    textLength += formattedText.WidthIncludingTrailingWhitespace;
                }

                GenerateVisualChildren();
            }
            catch (Exception e)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP01", e);
            }
        }

        protected virtual void GenerateVisualChildren()
        {
            try
            {
                visualChildren.Clear();

                foreach (FormattedText formText in formattedChars)
                {
                    DrawingVisual drawingVisual = new DrawingVisual();

                    TransformGroup transformGroup = new TransformGroup();
                    transformGroup.Children.Add(new ScaleTransform());
                    transformGroup.Children.Add(new RotateTransform());
                    transformGroup.Children.Add(new TranslateTransform());
                    drawingVisual.Transform = transformGroup;

                    DrawingContext dc = drawingVisual.RenderOpen();
                    dc.DrawText(formText, new Point(0, 0));
                    dc.Close();

                    visualChildren.Add(drawingVisual);
                }

                TransformVisualChildren();
            }
            catch (Exception e)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP02", e);
            }
        }

        protected virtual void TransformVisualChildren()
        {
            try
            {
                pathLength = GetPathFigureLength(PathFigure);
                boundingRect = new Rect();

                if (pathLength == 0 || textLength == 0)
                    return;

                if (formattedChars.Count != visualChildren.Count)
                    return;

                double scalingFactor = pathLength / textLength;
                PathGeometry pathGeometry =
                    new PathGeometry(new PathFigure[] { PathFigure });
                double progress = 0;
                boundingRect = new Rect();

                for (int index = 0; index < visualChildren.Count; index++)
                {
                    scalingFactor = 1;
                    FormattedText formText = formattedChars[index];
                    double width = scalingFactor *
                                formText.WidthIncludingTrailingWhitespace;
                    double baseline = scalingFactor * formText.Baseline;
                    progress += width / 2 / pathLength;
                    Point point, tangent;
                    pathGeometry.GetPointAtFractionLength(progress,
                                                out point, out tangent);

                    DrawingVisual drawingVisual =
                        visualChildren[index] as DrawingVisual;
                    TransformGroup transformGroup =
                        drawingVisual.Transform as TransformGroup;
                    ScaleTransform scaleTransform =
                        transformGroup.Children[0] as ScaleTransform;
                    RotateTransform rotateTransform =
                        transformGroup.Children[1] as RotateTransform;
                    TranslateTransform translateTransform =
                        transformGroup.Children[2] as TranslateTransform;

                    scaleTransform.ScaleX = scalingFactor;
                    scaleTransform.ScaleY = scalingFactor;
                    rotateTransform.Angle =
                        Math.Atan2(tangent.Y, tangent.X) * 180 / Math.PI;
                    rotateTransform.CenterX = width / 2;
                    rotateTransform.CenterY = baseline;
                    translateTransform.X = point.X - width / 2;
                    translateTransform.Y = point.Y - baseline;

                    Rect rect = drawingVisual.ContentBounds;
                    rect.Transform(transformGroup.Value);
                    boundingRect.Union(rect);

                    progress += width / 2 / pathLength;
                }
                InvalidateMeasure();
            }
            catch(Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TP03", ex);
            }
        }

        protected override int VisualChildrenCount
        {
            get
            {
                return visualChildren.Count;
            }
        }

        protected override Visual GetVisualChild(int index)
        {
            if (index < 0 || index >= visualChildren.Count)
                throw new ArgumentOutOfRangeException("index");

            return visualChildren[index];
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            return (Size)boundingRect.BottomRight;
        }
    }
}
