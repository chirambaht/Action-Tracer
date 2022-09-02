import numpy


class Filter:
    FILTER_TYPES = [ "mean", "median", "comparative" ]
    DEFAULT_WINDOW_SIZE = 100

    filterType = None
    filterBuffer = None
    filterColumns = None
    currentData = None
    compliementaryBias = 0.5

    def __init__( self, filterType, columns, windowSize=DEFAULT_WINDOW_SIZE ):
        if filterType in self.FILTER_TYPES:
            self.filterType = filterType
        assert type( columns ) == int
        self.filterColumns = columns
        self.filterBuffer = numpy.zeros( ( windowSize,  columns ) )
        self.currentData = numpy.zeros( ( 1, columns ) )

    def setComplimentaryBias( self, bias ):
        self.compliementaryBias = bias

    def filter( self, newData ):
        self.filterBuffer = numpy.delete( self.filterBuffer, 0, 0 )
        self.filterBuffer = numpy.append( self.filterBuffer, [ newData ], 0 )
        if self.filterType == "mean":
            self.currentData = numpy.mean( self.filterBuffer, axis=0 )
        elif self.filterType == "median":
            self.currentData = numpy.median( self.filterBuffer, axis=0 )
        elif self.filterType == "comparative":
            self.currentData = ( numpy.mean( self.filterBuffer, axis=0 ) * self.compliementaryBias ) + (
                numpy.median( self.filterBuffer, axis=0 ) * ( 1 - self.compliementaryBias ) )
        else:
            self.currentData = newData
        return self.currentData

    def getBufferData( self ):
        return self.currentData
    
    def getBufferDataAsList( self ):
        return self.currentData.tolist()
