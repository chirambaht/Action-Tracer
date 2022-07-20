import matplotlib.pyplot as plt
import numpy as np


def get_single_figure( data, x_axis, title="", y_label="", x_label="Time", save="" ):

    plt.rcParams[ 'figure.figsize' ] = ( 10, 6 )

    plt.plot( x_axis, data )
    plt.title( title )
    plt.xlabel( x_label )
    plt.ylabel( y_label )

    if len( save ) > 0:
        print( "Saving to ", save )
        plt.savefig( f"{save}.png" )
    else:
        plt.show()
    plt.close()
    return


def get_multiple_figures( data, x_axis, title="", y_label="", x_label="Time", save="" ):
    # make plot size bigger
    plt.rcParams[ 'figure.figsize' ] = ( 15, 19 )
    # adjust the subplot spacing
    plt.subplots_adjust( hspace=0.35 )
    plt.plot( x_axis, data )
    plt.title( title )
    plt.xlabel( x_label )
    plt.ylabel( y_label )

    if len( save ) > 0:
        plt.savefig( f"{title}.png" )
    else:
        plt.show()
    plt.close()
    return
