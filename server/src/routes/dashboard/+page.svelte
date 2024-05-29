<script lang="ts">
  import { getModalStore } from '@skeletonlabs/skeleton';
  import type { ModalSettings, ModalComponent, ModalStore } from '@skeletonlabs/skeleton';
  const modalStore = getModalStore();

  const modalAddDevice: ModalSettings = {
    backdropClasses: '!backdrop-blur-md',
	  modalClasses: '!bg-white',
    type: 'prompt',
    // Data
    title: 'Enter Device ID',
    body: 'Provide the MAC address of your device below, e.g. 202111579, 202112345',
    // Populates the input value and attributes
    value: '',
    valueAttr: { type: 'number', minlength: 10, maxlength: 10, required: true },
    // Returns the updated response value
    response: (r: string) => {
      addDeviceValue = r;
      console.log('response:', r);
    },
  };

  let addDeviceValue = '';
              
  type Device = {
    name: string;
    id: number;
    status: string;
    smoke: boolean;
  };

  let devices: Device[] = [
    { name: 'pips', id: 202111579, status: 'on', smoke: true },
    { name: 'lance', id: 201800866, status: 'on', smoke: false },
    { name: 'julia', id: 202108595, status: 'forced off', smoke: false },
    { name: 'renzo', id: 202100912, status: 'forced off', smoke: false },
    { name: 'marius', id: 202102371, status: 'off', smoke: false },
    { name: 'jaren', id: 202102804, status: 'off', smoke: false }
  ];

  export let data;

  $: ({ user, supabase } = data);

  let addDeviceForm: HTMLFormElement;

  async function addDevice() {
    const formData = new FormData(addDeviceForm);
    const macAddress = formData.get('macAddress') as string;

    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: macAddress
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }
</script>

<nav class='flex flex-row justify-between h-min w-full mb-6 mx-6'>
  <p class='text-lg' style='color: #E7191F;'>FireWall</p>
  <p class='text-lg color-black italic'>Hello, user!</p>
  <!-- user icon -->
    <!-- logout -->
</nav>
<div class='grid grid-flow-row grid-cols-1 lg:grid-cols-2 gap-4 overflow-y-scroll mx-6'>
  <div class='flex flex-row justify-between w-full h-min lg:col-span-2 items-center'>
    <h1 class='text-3xl'>Devices</h1>
    <form bind:this={addDeviceForm} on:submit|preventDefault={addDevice}>
      <label for="macAddress">
        <!-- CHANGE THIS -->
        <input name="macAddress" type="hidden" bind:value={addDeviceValue}/>
      </label>
      <button class='bg-red-400 p-4 rounded-lg w-40 h-min text-sm text-white' on:click={() => {modalStore.trigger(modalAddDevice);}}>+ Add Device</button>
    </form>
  </div>
  {#each devices as device (device.id)}
    <div class='w-auto h-40 p-4 items-center bg-gray-200 rounded-lg border border-gray-200'>
      <p class='w-min h-min'>{device.name}</p>
      <p class='w-min h-min'>{device.id}</p>
    </div>
  {/each}
</div>
<!-- top bar -->
    <!-- FireWall name -->
    <!-- user icon -->
        <!-- logout -->
<!-- devices -->
    <!-- title -->
        <!-- add device -->
    <!-- device list -->
        <!-- smoke detected -->
        <!-- plug icon -->
        <!-- name, number -->
        <!-- status -->