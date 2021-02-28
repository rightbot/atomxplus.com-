declare var isCI: boolean;
declare var standardScheme: string;

declare namespace Electron {
  interface Menu {
    delegate: {
      executeCommand(menu: Menu, event: any, id: number): void;
      menuWillShow(menu: Menu): void;
    };
    getAcceleratorTextAt(index: number): string;
  }

  interface MenuItem {
    getDefaultRoleAccelerator(): Accelerator | undefined;
  }

  interface WebContents {
    getOwnerBrowserWindow(): BrowserWindow;
  }

  interface Session {
    destroy(): void;
  }

  // Experimental views API
  class TopLevelWindow {
    constructor(args: {show: boolean})
    setContentView(view: View): void
  }
  class View {}
}

declare module 'dbus-native';
